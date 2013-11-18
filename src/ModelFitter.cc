#include "ModelFitter.h"

#include "MarkerModel.h"
#include "MarkerData.h"

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
		unsigned int max_iter
		) {

	assert (Qinit.size() == model.q_size);
	assert (body_id.size() == body_point.size());
	assert (body_id.size() == target_pos.size());

	rbdlMatrixNd J = rbdlMatrixNd::Zero(3 * body_id.size(), model.qdot_size);
	rbdlVectorNd e = rbdlVectorNd::Zero(3 * body_id.size());

	Qres = Qinit;

	for (unsigned int ik_iter = 0; ik_iter < max_iter; ik_iter++) {
		UpdateKinematicsCustom (model, &Qres, NULL, NULL);

		for (unsigned int k = 0; k < body_id.size(); k++) {
			rbdlMatrixNd G (3, model.qdot_size);
			CalcPointJacobian (model, Qres, body_id[k], body_point[k], G, false);
			rbdlVector3d point_base = CalcBodyToBaseCoordinates (model, Qres, body_id[k], body_point[k], false);
			LOG << "current_pos = " << point_base.transpose() << std::endl;

			for (unsigned int i = 0; i < 3; i++) {
				for (unsigned int j = 0; j < model.qdot_size; j++) {
					unsigned int row = k * 3 + i;
					LOG << "i = " << i << " j = " << j << " k = " << k << " row = " << row << " col = " << j << std::endl;
					J(row, j) = G (i,j);
				}

				e[k * 3 + i] = target_pos[k][i] - point_base[i];
			}

			LOG << J << std::endl;

			// abort if we are getting "close"
			if (e.norm() < step_tol) {
				LOG << "Reached target close enough after " << ik_iter << " steps" << std::endl;
				return true;
			}
		}

		LOG << "J = " << J << std::endl;
		LOG << "e = " << e.transpose() << std::endl;

		double ek = e.squaredNorm() * 0.5;
		double wn = 1.0e-3;
		rbdlMatrixNd JJT_ek_wnI = J * J.transpose() + (ek + wn) * rbdlMatrixNd::Identity(e.size(), e.size());
//		rbdlMatrixNd JJTe_lambda2_I = J * J.transpose() + lambda*lambda * rbdlMatrixNd::Identity(e.size(), e.size());

		rbdlVectorNd z (body_id.size() * 3);
#ifndef RBDL_USE_SIMPLE_MATH
		z = JJT_ek_wnI.colPivHouseholderQr().solve (e);
#else
		bool solve_successful = LinSolveGaussElimPivot (JJT_ek_wnI, e, z);
		assert (solve_successful);
#endif

		LOG << "z = " << z << std::endl;

		rbdlVectorNd delta_theta = J.transpose() * z;
		LOG << "change = " << delta_theta << std::endl;

		Qres = Qres + delta_theta;
		LOG << "Qres = " << Qres.transpose() << std::endl;

		if (delta_theta.norm() < step_tol) {
			LOG << "reached convergence after " << ik_iter << " steps" << std::endl;
			cout << "IK result ||e|| = " << e.norm() << " steps: " << ik_iter << endl;
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

		LOG << "test_res = " << test_res.transpose() << std::endl;
	}

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

bool ModelFitter::run(const VectorNd &initialState) {
	fittedState = initialState;
	success = false;

	rbdlVectorNd Qinit = ConvertVector<rbdlVectorNd, VectorNd> (initialState);
	rbdlVectorNd Qres = ConvertVector<rbdlVectorNd, VectorNd> (initialState);

	vector<unsigned int> body_ids;
	vector<rbdlVector3d> body_points;
	vector<rbdlVector3d> target_pos;

	int frame_count = model->getFrameCount();
	for (int frame_id = 1; frame_id <= frame_count; frame_id++) {
		unsigned int body_id = model->frameIdToRbdlId[frame_id];
		vector<Vector3f> marker_coords = model->getFrameMarkerCoords(frame_id);
		vector<string> marker_names = model->getFrameMarkerNames(frame_id);

		assert (marker_coords.size() == marker_names.size());

		for (size_t marker_idx = 0; marker_idx < marker_coords.size(); marker_idx++) {
			body_ids.push_back (body_id);
			body_points.push_back (ConvertVector<rbdlVector3d, Vector3d> (marker_coords[marker_idx]));
			target_pos.push_back (ConvertVector<rbdlVector3d, Vector3d> (data->getMarkerCurrentPosition (marker_names[marker_idx].c_str())));
		}
	}

	success = SugiharaIK (*(model->rbdlModel), Qinit, body_ids, body_points, target_pos, Qres, 1.0e-8, 150);
//	success = RigidBodyDynamics::InverseKinematics (*(model->rbdlModel), Qinit, body_ids, body_points, target_pos, Qres, 1.0e-12, 0.05, 100);
	fittedState = ConvertVector<VectorNd, rbdlVectorNd> (Qres);

	model->modelStateQ = fittedState;

	vector<Vector3f> marker_displacement;
	VectorNd marker_errors (VectorNd::Zero (body_ids.size()));
	int index = 0;
	double max_error = 0.;
	string max_error_marker_name = "";
	double error_norm_sum = 0.;
	for (int frame_id = 0; frame_id < frame_count; frame_id++) {
		vector<string> marker_names = model->getFrameMarkerNames(frame_id);

		for (size_t marker_idx = 0; marker_idx < marker_names.size(); marker_idx++) {
			Vector3f marker_position_data = data->getMarkerCurrentPosition (marker_names[marker_idx].c_str());
			Vector3f marker_position_model = model->getMarkerPosition (frame_id, marker_names[marker_idx].c_str());

			Vector3d error = marker_position_data - marker_position_model;
			marker_displacement.push_back (error);
			double error_norm = error.norm();
			marker_errors[index] = error_norm;
			if (error_norm > 0.02) {
				cout << "error: " << marker_names[marker_idx] << " = " << error_norm << endl;
			}
			if (error_norm > max_error) {
				max_error = error_norm;
				max_error_marker_name = marker_names[marker_idx];
			}
			error_norm_sum += error_norm * error_norm;
			index++;
		}
	}

	double rms_error = sqrt(error_norm_sum / index);
	cout << "Errors: max = " << max_error << " " << max_error_marker_name << " avg = " << error_norm_sum / index << " rms = " << rms_error << endl; // << marker_errors.transpose() << endl;
	cout << "Average: " << vec_average (marker_errors) << " deviation = " << vec_standard_deviation (marker_errors) << endl;

	return success;
}
