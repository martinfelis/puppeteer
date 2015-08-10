/* 
 * Puppeteer - A Motion Capture Mapping Tool
 * Copyright (c) 2013-2015 Martin Felis <martin.felis@iwr.uni-heidelberg.de>.
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE* 
 */

#include "ModelFitter.h"

#include "Model.h"
#include "MarkerData.h"
#include "Animation.h"
#include <fstream>
#include <rbdl/rbdl.h>

using namespace std;

typedef RigidBodyDynamics::Math::Vector3d rbdlVector3d;
typedef RigidBodyDynamics::Math::VectorNd rbdlVectorNd;
typedef RigidBodyDynamics::Math::MatrixNd rbdlMatrixNd;

template <typename OutType, typename InType>
OutType ConvertVector(const InType &in_vec) {
	OutType result = OutType::Zero (in_vec.size());
	for (size_t i = 0; i < in_vec.size(); i++) {
		result[i] = in_vec[i];
	}

	return result;
}

struct ModelFitter::ModelFitterInternal {
	rbdlVectorNd Qinit;
	rbdlVectorNd Qres;
	std::map<std::string, int> marker_residual_index;
	std::vector<unsigned int> body_ids;
	std::vector<rbdlVector3d> body_points;
	std::vector<rbdlVector3d> target_pos;
	std::vector<string> marker_names;
};

ModelFitter::ModelFitter() {
	internal = new ModelFitterInternal();
}

ModelFitter::ModelFitter (Model *model, MarkerData *data, unsigned int maxSteps) :
		model (model),
		data (data),
		tolerance (1.0e-8),
		success (false),
		steps (0),
		maxSteps (maxSteps)
	{
		internal = new ModelFitterInternal();
	}

ModelFitter::~ModelFitter() {
	delete internal;
}

/** Inverse Kinematics using a Levenberg Marquardt with constant lambda
 */
bool LevenbergMarquardtIK (
		RigidBodyDynamics::Model &model,
		const rbdlVectorNd &Qinit,
		const std::vector<unsigned int>& body_id,
		const std::vector<rbdlVector3d>& body_point,
		const std::vector<rbdlVector3d>& target_pos,
		rbdlVectorNd &Qres,
		double step_tol,
		double lambda,
		unsigned int max_iter,
		unsigned int *steps,
		VectorNd *residuals
		) {

	assert (Qinit.size() == model.q_size);
	assert (body_id.size() == body_point.size());
	assert (body_id.size() == target_pos.size());

	rbdlMatrixNd J = rbdlMatrixNd::Zero(3 * body_id.size(), model.qdot_size);
	rbdlVectorNd e = rbdlVectorNd::Zero(3 * body_id.size());

	Qres = Qinit;

	unsigned int ik_iter;

	for (ik_iter = 0; ik_iter < max_iter; ik_iter++) {
		UpdateKinematicsCustom (model, &Qres, NULL, NULL);

		for (unsigned int k = 0; k < body_id.size(); k++) {
			rbdlMatrixNd G (rbdlMatrixNd::Zero(3, model.qdot_size));
			CalcPointJacobian (model, Qres, body_id[k], body_point[k], G, false);
			rbdlVector3d point_base = CalcBodyToBaseCoordinates (model, Qres, body_id[k], body_point[k], false);

			for (unsigned int i = 0; i < 3; i++) {
				for (unsigned int j = 0; j < model.qdot_size; j++) {
					unsigned int row = k * 3 + i;
					J(row, j) = G (i,j);
				}

				e[k * 3 + i] = target_pos[k][i] - point_base[i];
			}

			// abort if we are getting "close"
			if (e.norm() < step_tol) {
				*residuals = ConvertVector<VectorNd, rbdlVectorNd> (e);
				*steps = ik_iter;

				return true;
			}
		}

		rbdlMatrixNd JJTe_lambda2_I = J * J.transpose() + lambda*lambda * rbdlMatrixNd::Identity(e.size(), e.size());

		rbdlVectorNd z (body_id.size() * 3);
		z = JJTe_lambda2_I.colPivHouseholderQr().solve (e);

		rbdlVectorNd delta_theta = J.transpose() * z;
		Qres = Qres + delta_theta;

		if (delta_theta.norm() < step_tol) {
			*residuals = ConvertVector<VectorNd, rbdlVectorNd> (e);
			*steps = ik_iter;
			return true;
		}
	}

	*residuals = ConvertVector<VectorNd, rbdlVectorNd> (e);
	*steps = ik_iter;

	return false;
}

/** Inverse Kinematics method by Sugihara
 *
 * Sugihara, T., "Solvability-Unconcerned Inverse Kinematics by the
 * Levenberg–Marquardt Method," Robotics, IEEE Transactions on , vol.27, no.5,
 * pp.984,991, Oct. 2011 doi: 10.1109/TRO.2011.2148230
 */
bool SugiharaIK (
		RigidBodyDynamics::Model &model,
		const rbdlVectorNd &Qinit,
		const std::vector<unsigned int>& body_id,
		const std::vector<rbdlVector3d>& body_point,
		const std::vector<rbdlVector3d>& target_pos,
		rbdlVectorNd &Qres,
		double step_tol,
		unsigned int max_iter,
		unsigned int *steps,
		VectorNd *residuals
		) {

	assert (Qinit.size() == model.q_size);
	assert (body_id.size() == body_point.size());
	assert (body_id.size() == target_pos.size());

	rbdlMatrixNd J = rbdlMatrixNd::Zero(3 * body_id.size(), model.qdot_size);
	rbdlVectorNd e = rbdlVectorNd::Zero(3 * body_id.size());

	Qres = Qinit;

	unsigned int ik_iter;

	for (ik_iter = 0; ik_iter < max_iter; ik_iter++) {
		UpdateKinematicsCustom (model, &Qres, NULL, NULL);

		for (unsigned int k = 0; k < body_id.size(); k++) {
			rbdlMatrixNd G (rbdlMatrixNd::Zero(3, model.qdot_size));
			CalcPointJacobian (model, Qres, body_id[k], body_point[k], G, false);
			rbdlVector3d point_base = CalcBodyToBaseCoordinates (model, Qres, body_id[k], body_point[k], false);

			for (unsigned int i = 0; i < 3; i++) {
				for (unsigned int j = 0; j < model.qdot_size; j++) {
					unsigned int row = k * 3 + i;
					J(row, j) = G (i,j);
				}

				e[k * 3 + i] = target_pos[k][i] - point_base[i];
			}
		}

		double wn = 1.0e-3;
		double Ek = 0.;

		for (size_t ei = 0; ei < e.size(); ei ++) {
			Ek += e[ei] * e[ei] * 0.5;
		}

		rbdlVectorNd ek = J.transpose() * e;
		rbdlMatrixNd Wn = rbdlMatrixNd::Zero (Qres.size(), Qres.size());

		assert (ek.size() == Qres.size());

		for (size_t wi = 0; wi < Qres.size(); wi++) {
//			Wn(wi, wi) = ek[wi] * ek[wi] * 0.5 + 1.0e-3;
			Wn(wi, wi) = Ek + 1.0e-3;
		}

		rbdlMatrixNd A = J.transpose() * J + Wn;
		rbdlVectorNd delta_theta = A.colPivHouseholderQr().solve(J.transpose() * e);
		Qres = Qres + delta_theta;

		if (delta_theta.norm() < step_tol) {
			*residuals = ConvertVector<VectorNd, rbdlVectorNd> (e);
			*steps = ik_iter;

			return true;
		}
	}

	*residuals = ConvertVector<VectorNd, rbdlVectorNd> (e);
	*steps = ik_iter;

	return false;
}

bool SugiharaTaskSpaceIK (
		RigidBodyDynamics::Model &model,
		const rbdlVectorNd &Qinit,
		const std::vector<unsigned int>& body_id,
		const std::vector<rbdlVector3d>& body_point,
		const std::vector<rbdlVector3d>& target_pos,
		rbdlVectorNd &Qres,
		double step_tol,
		unsigned int max_iter,
		unsigned int *steps,
		VectorNd *residuals
		) {

	assert (Qinit.size() == model.q_size);
	assert (body_id.size() == body_point.size());
	assert (body_id.size() == target_pos.size());

	rbdlMatrixNd J = rbdlMatrixNd::Zero(3 * body_id.size(), model.qdot_size);
	rbdlVectorNd e = rbdlVectorNd::Zero(3 * body_id.size());

	Qres = Qinit;

	unsigned int ik_iter;

	for (ik_iter = 0; ik_iter < max_iter; ik_iter++) {
		UpdateKinematicsCustom (model, &Qres, NULL, NULL);

		for (unsigned int k = 0; k < body_id.size(); k++) {
			rbdlMatrixNd G (rbdlMatrixNd::Zero(3, model.qdot_size));
			CalcPointJacobian (model, Qres, body_id[k], body_point[k], G, false);
			rbdlVector3d point_base = CalcBodyToBaseCoordinates (model, Qres, body_id[k], body_point[k], false);

			for (unsigned int i = 0; i < 3; i++) {
				for (unsigned int j = 0; j < model.qdot_size; j++) {
					unsigned int row = k * 3 + i;
					J(row, j) = G (i,j);
				}

				e[k * 3 + i] = target_pos[k][i] - point_base[i];
			}

			// abort if we are getting "close"
			if (e.norm() < step_tol) {
				*residuals = ConvertVector<VectorNd, rbdlVectorNd> (e);
				*steps = ik_iter;

				return true;
			}
		}

		double wn = 1.0e-3;
		rbdlMatrixNd Ek = rbdlMatrixNd::Zero (e.size(), e.size());
	
		for (size_t ei = 0; ei < e.size(); ei ++) {
			Ek(ei,ei) = e[ei] * e[ei] * 0.5 + wn;
		}

		rbdlMatrixNd JJT_Ek_wnI = J * J.transpose() + Ek;

		rbdlVectorNd delta_theta = J.transpose() * JJT_Ek_wnI.colPivHouseholderQr().solve (e);

		Qres = Qres + delta_theta;
		if (delta_theta.norm() < step_tol) {
			*residuals = ConvertVector<VectorNd, rbdlVectorNd> (e);
			*steps = ik_iter;

			return true;
		}
	}

	*residuals = ConvertVector<VectorNd, rbdlVectorNd> (e);
	*steps = ik_iter;

	return false;
}

double vec_average (const VectorNd &vec) {
	double sum = 0.;
	for (size_t i = 0; i < vec.size(); i++) 
		sum += vec[i];

	return sum / vec.size();
}

double vec_standard_deviation (const VectorNd &vec) {
	double average = vec_average (vec);
	double diff_sum = 0.;
	for (size_t i = 0; i < vec.size(); i++) {
		diff_sum += (vec[i] - average) * (vec[i] - average);
	}

	return sqrt (diff_sum / vec.size());
}

void ModelFitter::setup() {
	fittedState = initialState;
	success = false;

	internal->Qinit = ConvertVector<rbdlVectorNd, VectorNd> (initialState);
	internal->Qres = ConvertVector<rbdlVectorNd, VectorNd> (initialState);
	residuals = VectorNd::Zero (initialState.size());

	internal->body_ids.clear();
	internal->body_points.clear();
	internal->target_pos.clear();
	internal->marker_residual_index.clear();
	internal->marker_names.clear();

	int frame_count = model->getFrameCount();
	int residual_index = 0;

	for (int frame_id = 1; frame_id <= frame_count; frame_id++) {
		unsigned int body_id = model->frameIdToRbdlId[frame_id];
		vector<Vector3f> marker_coords = model->getFrameMarkerCoords(frame_id);
		vector<string> marker_names = model->getFrameMarkerNames(frame_id);

		assert (marker_coords.size() == marker_names.size());

		for (size_t marker_idx = 0; marker_idx < marker_coords.size(); marker_idx++) {
			rbdlVector3d marker_data_pos = ConvertVector<rbdlVector3d, Vector3d> (data->getMarkerCurrentPosition (marker_names[marker_idx].c_str()));
			internal->marker_names.push_back(marker_names[marker_idx]);

			if (marker_data_pos == rbdlVector3d (0., 0., 0.) || marker_data_pos.squaredNorm() > 1.0e2) {
				cerr << "Warning: invalid marker data for marker '" << marker_names[marker_idx] << "' at frame " << data->currentFrame << ". Not fitting to this marker." << endl;
				internal->marker_residual_index[marker_names[marker_idx]] = -1;
				continue;
			}

			internal->marker_residual_index[marker_names[marker_idx]] = residual_index;
			residual_index++;

			internal->body_ids.push_back (body_id);
			internal->body_points.push_back (ConvertVector<rbdlVector3d, Vector3d> (marker_coords[marker_idx]));

			internal->target_pos.push_back (marker_data_pos);
		}
	}
}

bool ModelFitter::computeModelAnimationFromMarkers (const VectorNd &_initialState, Animation *animation, int frame_start, int frame_end) {
	assert (model);
	assert (data);
	assert (animation);

	int old_current_frame = data->currentFrame;

	double current_time = 0.;
	double frame_rate = static_cast<double>(data->getFrameRate());
	int frame_first = data->getFirstFrame();
	int frame_last = data->getLastFrame();
	double data_duration = static_cast<double>(frame_last - frame_first) / frame_rate;
	bool result = true;

	if (frame_start == -1 || frame_start < frame_first)
		frame_start = frame_first;
	if (frame_end == -1 || frame_end > frame_last)
		frame_end = frame_last;

	ofstream iklog;
	if (frame_start == frame_first) {
		iklog.open("fitting_log.csv");
		setup();
	
		iklog << "frame, steps, ";
		for (size_t i = 0; i < internal->marker_names.size(); i++) {
			iklog << internal->marker_names[i];
			if (i != internal->marker_names.size() - 1)
				iklog << ", ";
		}
		iklog << endl;
	}	else {
		iklog.open("fitting_log.csv", ios::app);
	}

	VectorNd current_state = _initialState;

	for (int i = frame_start; i <= frame_end; i++) {
		current_time = static_cast<double>(i - frame_first) / static_cast<double>(frame_last - frame_first) * data_duration;
		data->setCurrentFrameNumber (i);

		if (!run (current_state)) {
			result = false;
			cerr << "Warning: could not fit frame " << i << endl;
		}

		iklog << i - frame_first << ", ";
		iklog << steps << ", ";

		for (size_t mi = 0; mi < internal->marker_names.size(); mi++) {
			int ri = internal->marker_residual_index[internal->marker_names[mi]];
			if (ri == -1) {
				iklog << 0.;
			} else {
				Vector3d marker_res (residuals[ri * 3], residuals[ri * 3 + 1], residuals[ri * 3 + 2]); 
				iklog << marker_res.norm();
			}

			if (mi != internal->marker_names.size() - 1)
				iklog << ", ";
		}
		iklog << endl;

		current_state = getFittedState();
		animation->addPose (current_time, current_state);
	}
	iklog.close();

	data->currentFrame = old_current_frame;

	return result;
}

void ModelFitter::analyzeAnimation (Animation animation) {
	assert (model);
	assert (data);

	double frame_rate = static_cast<double>(data->getFrameRate());
	int frame_first = data->getFirstFrame();
	int frame_last = data->getLastFrame();
	double data_duration = static_cast<double>(frame_last - frame_first) / frame_rate;

	assert (data_duration == animation.getDuration());

	ofstream iklog;

	iklog.open("fitting_log.csv");
	setup();

	iklog << "frame, steps, ";
	for (size_t i = 0; i < internal->marker_names.size(); i++) {
		iklog << internal->marker_names[i];
		if (i != internal->marker_names.size() - 1)
			iklog << ", ";
	}
	iklog << endl;

	for (int i = frame_first; i <= frame_last; i++) {
		double current_time = static_cast<double>(i - frame_first) / static_cast<double>(frame_last - frame_first) * data_duration;
		data->setCurrentFrameNumber (i);
		animation.setCurrentTime (current_time);
		rbdlVectorNd q = ConvertVector<rbdlVectorNd, VectorNd>(animation.getCurrentPose());

		iklog << i - frame_first << ", ";
		iklog << 0 << ", ";
		
		setup();

		UpdateKinematicsCustom (*(model->rbdlModel), &q, NULL, NULL);

		for (size_t mi = 0; mi < internal->marker_names.size(); mi++) {
			string marker_name = internal->marker_names[mi];
			int ri = internal->marker_residual_index[internal->marker_names[mi]];
			if (ri == -1) {
				iklog << 0.;
			} else {
				rbdlVector3d data_marker = internal->target_pos[mi];
				rbdlVector3d model_marker = CalcBodyToBaseCoordinates (*(model->rbdlModel), q, internal->body_ids[mi], internal->body_points[mi], false);
				iklog << (data_marker - model_marker).norm();
			}

			if (mi != internal->marker_names.size() - 1)
				iklog << ", ";
		}
		iklog << endl;
	}
	iklog.close();
}

bool LevenbergMarquardtFitter::run (const VectorNd &_initialState) {
	initialState = _initialState;

	setup();

	success = LevenbergMarquardtIK (*(model->rbdlModel), internal->Qinit, internal->body_ids, internal->body_points, internal->target_pos, internal->Qres, tolerance, lambda, maxSteps, &steps, &residuals);
	fittedState = ConvertVector<VectorNd, rbdlVectorNd> (internal->Qres);

	return success;
}

bool SugiharaFitter::run (const VectorNd &_initialState) {
	initialState = _initialState;

	setup();

	success = SugiharaIK (*(model->rbdlModel), internal->Qinit, internal->body_ids, internal->body_points, internal->target_pos, internal->Qres, tolerance, maxSteps, &steps, &residuals);
	fittedState = ConvertVector<VectorNd, rbdlVectorNd> (internal->Qres);

	return success;
}

bool SugiharaTaskSpaceFitter::run (const VectorNd &_initialState) {
	initialState = _initialState;

	setup();

	success = SugiharaTaskSpaceIK (*(model->rbdlModel), internal->Qinit, internal->body_ids, internal->body_points, internal->target_pos, internal->Qres, tolerance, maxSteps, &steps, &residuals);
	fittedState = ConvertVector<VectorNd, rbdlVectorNd> (internal->Qres);

	return success;
}
