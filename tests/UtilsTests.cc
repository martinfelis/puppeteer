#include <UnitTest++.h>

#include "SimpleMath/SimpleMathGL.h"
#include "Scene.h"

#include <iostream>

using namespace std;

const float TEST_PREC = 1.0e-6;

TEST ( IdToVector4ConversionZeroTest ) {
	int id = 254;
	Vector4f color = object_id_to_vector4 (id);
	CHECK_EQUAL (id, vector4_to_object_id (color));

	id = 255;
	color = object_id_to_vector4 (id);
	CHECK_EQUAL (id, vector4_to_object_id (color));

	id = 256;
	color = object_id_to_vector4 (id);
	CHECK_EQUAL (id, vector4_to_object_id (color));

	id = 65534;
	color = object_id_to_vector4 (id);
	CHECK_EQUAL (id, vector4_to_object_id (color));

	id = 65535;
	color = object_id_to_vector4 (id);
	CHECK_EQUAL (id, vector4_to_object_id (color));
}
