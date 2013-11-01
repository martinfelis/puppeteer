#include <UnitTest++.h>

#include "SimpleMath/SimpleMathGL.h"
#include "Scene.h"

#include <iostream>

using namespace std;

using namespace SimpleMath;
using namespace SimpleMath::GL;

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

TEST ( QuaternionToEulerZYXTests ) {
	Vector3f zyx_euler (1.1f, 1.3f, 1.9f);

	Quaternion q_euler = Quaternion::fromEulerZYX (zyx_euler);
	Vector3f euler_from_q = q_euler.toEulerZYX();

	CHECK_ARRAY_CLOSE (zyx_euler.data(), euler_from_q.data(), 3, TEST_PREC);
}

TEST ( QuaternionToEulerYXZTests ) {
	Vector3f yxz_euler (1.1f, 1.3f, 1.9f);

	Quaternion q_euler = Quaternion::fromEulerYXZ (yxz_euler);
	Vector3f euler_from_q = q_euler.toEulerYXZ();

	CHECK_ARRAY_CLOSE (yxz_euler.data(), euler_from_q.data(), 3, TEST_PREC);
}
