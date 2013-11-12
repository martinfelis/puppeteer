#include "ModelFitter.h"

#include "MarkerModel.h"
#include "MarkerData.h"

#include <rbdl/rbdl.h>

using namespace std;
using namespace RigidBodyDynamics;

typedef RigidBodyDynamics::Math::Vector3d rbdlVector3d;
typedef RigidBodyDynamics::Math::VectorNd rbdlVectorNd;

template <typename OutType, typename InType>
OutType ConvertVector(const InType &in_vec) {
	OutType result = OutType::Zero (in_vec.size());
	for (int i = 0; i < in_vec.size(); i++) {
		result[i] = in_vec[i];
	}

	return result;
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
	for (int frame_id = 0; frame_id < frame_count; frame_id++) {
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

	success = RigidBodyDynamics::InverseKinematics (*(model->rbdlModel), Qinit, body_ids, body_points, target_pos, Qres);

	fittedState = ConvertVector<VectorNd, rbdlVectorNd> (Qres);

	return success;
}
