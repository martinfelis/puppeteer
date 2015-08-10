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
