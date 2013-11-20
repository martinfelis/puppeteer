#include "ModelFitter.h"

#include "MarkerModel.h"
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
	std::vector<unsigned int> body_ids;
	std::vector<rbdlVector3d> body_points;
	std::vector<rbdlVector3d> target_pos;
};

ModelFitter::ModelFitter() {
	internal = new ModelFitterInternal();
}

ModelFitter::ModelFitter (MarkerModel *model, MarkerData *data) :
		model (model),
		data (data),
		tolerance (1.0e-8),
		success (false),
		steps (0)
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
			rbdlMatrixNd G (3, model.qdot_size);
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
#ifndef RBDL_USE_SIMPLE_MATH
		z = JJTe_lambda2_I.colPivHouseholderQr().solve (e);
#else
		bool solve_successful = LinSolveGaussElimPivot (JJTe_lambda2_I, e, z);
		assert (solve_successful);
#endif

		rbdlVectorNd delta_theta = J.transpose() * z;
		Qres = Qres + delta_theta;

		if (delta_theta.norm() < step_tol) {
			*residuals = ConvertVector<VectorNd, rbdlVectorNd> (e);
			*steps = ik_iter;
			return true;
		}

		rbdlVectorNd test_1 (z.size());
		rbdlVectorNd test_res (z.size());

		test_1.setZero();

		for (unsigned int i = 0; i < z.size(); i++) {
			test_1[i] = 1.;
			rbdlVectorNd test_delta = J.transpose() * test_1;
			test_res[i] = test_delta.squaredNorm();
			test_1[i] = 0.;
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
			rbdlMatrixNd G (3, model.qdot_size);
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

		double ek = e.squaredNorm() * 0.5;
		double wn = 1.0e-3;
		rbdlMatrixNd Ek = rbdlMatrixNd::Identity (e.size(), e.size());
	
		for (size_t ei = 0; ei < e.size(); ei ++) {
			Ek(ei,ei) = e[ei] * e[ei] * 0.5;
		}

		rbdlMatrixNd JJT_ek_wnI = J * J.transpose() + Ek + (wn) * rbdlMatrixNd::Identity(e.size(), e.size());

		rbdlVectorNd z (body_id.size() * 3);
#ifndef RBDL_USE_SIMPLE_MATH
		z = JJT_ek_wnI.colPivHouseholderQr().solve (e);
#else
		bool solve_successful = LinSolveGaussElimPivot (JJT_ek_wnI, e, z);
		assert (solve_successful);
#endif

		rbdlVectorNd delta_theta = J.transpose() * z;
		Qres = Qres + delta_theta;

		if (delta_theta.norm() < step_tol) {
//			cout << "IK result ||e|| = " << e.norm() << " steps: " << ik_iter << endl;
			*residuals = ConvertVector<VectorNd, rbdlVectorNd> (e);
			*steps = ik_iter;

			return true;
		}

		rbdlVectorNd test_1 (z.size());
		rbdlVectorNd test_res (z.size());

		test_1.setZero();

		for (unsigned int i = 0; i < z.size(); i++) {
			test_1[i] = 1.;
			rbdlVectorNd test_delta = J.transpose() * test_1;
			test_res[i] = test_delta.squaredNorm();
			test_1[i] = 0.;
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

	int frame_count = model->getFrameCount();
	for (int frame_id = 1; frame_id <= frame_count; frame_id++) {
		unsigned int body_id = model->frameIdToRbdlId[frame_id];
		vector<Vector3f> marker_coords = model->getFrameMarkerCoords(frame_id);
		vector<string> marker_names = model->getFrameMarkerNames(frame_id);

		assert (marker_coords.size() == marker_names.size());

		for (size_t marker_idx = 0; marker_idx < marker_coords.size(); marker_idx++) {
			internal->body_ids.push_back (body_id);
			internal->body_points.push_back (ConvertVector<rbdlVector3d, Vector3d> (marker_coords[marker_idx]));
			internal->target_pos.push_back (ConvertVector<rbdlVector3d, Vector3d> (data->getMarkerCurrentPosition (marker_names[marker_idx].c_str())));
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
		iklog.open("fitting.log");
	}	else {
		iklog.open("fitting.log", ios::app);
	}

	VectorNd current_state = _initialState;

	for (int i = frame_start; i <= frame_end; i++) {
		current_time = static_cast<double>(i - frame_first) / static_cast<double>(frame_last - frame_first) * data_duration;
		data->setCurrentFrameNumber (i);

		if (!run (current_state)) {
			result = false;
			cerr << "Warning: could not fit frame " << i << endl;
		}

		iklog << frame_start - frame_first << ", ";
		iklog << steps << ", ";
		for (int ri = 0; ri < residuals.size() / 3; ri++) {
			Vector3d marker_res (residuals[ri * 3], residuals[ri * 3 + 1], residuals[ri * 3 + 2]); 
			iklog << marker_res.norm();

			if (ri != (residuals.size() / 3) - 1)
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

bool LevenbergMarquardtFitter::run (const VectorNd &_initialState) {
	initialState = _initialState;

	setup();

	success = LevenbergMarquardtIK (*(model->rbdlModel), internal->Qinit, internal->body_ids, internal->body_points, internal->target_pos, internal->Qres, tolerance, lambda, 100, &steps, &residuals);
	fittedState = ConvertVector<VectorNd, rbdlVectorNd> (internal->Qres);

	return success;
}

bool SugiharaFitter::run (const VectorNd &_initialState) {
	initialState = _initialState;

	setup();

	success = SugiharaIK (*(model->rbdlModel), internal->Qinit, internal->body_ids, internal->body_points, internal->target_pos, internal->Qres, tolerance, 100, &steps, &residuals);
	fittedState = ConvertVector<VectorNd, rbdlVectorNd> (internal->Qres);

	return success;
}
