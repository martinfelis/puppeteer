#include <UnitTest++.h>

#include "c3dfile.h"

#include <iostream>

using namespace std;

const float TEST_PREC = 1.0e-6;

const char* filename = "../data/testdata.c3d";

TEST ( TestSimple ) {
	C3DFile c3dfile;
	CHECK (c3dfile.load (filename));
}

TEST ( TestGetParamSint16 ) {
	C3DFile c3dfile;
	c3dfile.load(filename);

	float camera_rate = c3dfile.getParamFloat("TRIAL:CAMERA_RATE");

	CHECK_EQUAL (100.f, camera_rate);
}

TEST ( TestGetParamString ) {
	C3DFile c3dfile;
	c3dfile.load(filename);

	std::string manufacturer_software = c3dfile.getParamString ("MANUFACTURER:SOFTWARE");

	CHECK_EQUAL (std::string("Vicon Nexus"), manufacturer_software);
}

TEST ( TestMarkerId ) {
	C3DFile c3dfile;
	c3dfile.load(filename);

	Sint16 lasi_marker_id = c3dfile.label_point_map["LASI"];

	CHECK_EQUAL (97, lasi_marker_id);
}

TEST ( TestPointValue ) {
	C3DFile c3dfile;
	c3dfile.load(filename);

	float lfhd_first[3] = {-491.922,	184.176,	1745.53};
	float lfhd_last[3] = {1816.24,	85.4363,	1725.09};

	FloatMarkerData lfhd_trajs = c3dfile.getMarkerTrajectories ("LFHD");
	
	size_t last_index = lfhd_trajs.x.size() - 1;
	float data_first[3] = { lfhd_trajs.x[0], lfhd_trajs.y[0], lfhd_trajs.z[0] };
	float data_last[3] = { lfhd_trajs.x[last_index], lfhd_trajs.y[last_index], lfhd_trajs.z[last_index] };

	CHECK_ARRAY_CLOSE (lfhd_first, data_first, 3, 1.0e-2);
	CHECK_ARRAY_CLOSE (lfhd_last, data_last, 3, 1.0e-2);
}
