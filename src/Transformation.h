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

#ifndef _TRANSFORMATION_H
#define _TRANSFORMATION_H

#include "SimpleMath/SimpleMath.h"
#include "SimpleMath/SimpleMathGL.h"

#include <iostream>

struct Transformation {
	Vector3f translation;
	SimpleMath::GL::Quaternion rotation;
	Vector3f scaling;

	Transformation() : 
		translation (0.f, 0.f, 0.f),
		rotation (0.f, 0.f, 0.f, 1.f),
		scaling (1.f, 1.f, 1.f)
	{}
	Transformation ( const Vector3f &translation, const SimpleMath::GL::Quaternion& rotation = SimpleMath::GL::Quaternion(0., 0., 0., 1.)) :
		translation (translation),
		rotation (rotation),
		scaling (1.f, 1.f, 1.f)
	{}
	Transformation ( const Vector3f &translation, const Matrix33f rotation_mat) :
		translation (translation),
		rotation (SimpleMath::GL::Quaternion::fromMatrix(rotation_mat)),
		scaling (1.f, 1.f, 1.f)
	{}

	Matrix44f toGLMatrix() const {
		return SimpleMath::GL::ScaleMat44 (scaling[0], scaling[1], scaling[2])
			* rotation.toGLMatrix() 
			* SimpleMath::GL::TranslateMat44 (translation[0], translation[1], translation[2]);
	}
	Transformation operator* (const Transformation &transform) const {
		Transformation result (*this);

		result.translation = transform.rotation.rotate(translation) + transform.translation ;
		result.scaling = Vector3f (
				transform.scaling[0] * scaling[0], 
				transform.scaling[1] * scaling[1], 
				transform.scaling[2] * scaling[2]
				);
		result.rotation = transform.rotation * rotation;

		return result;
	}
	Transformation inverse() const {
		Transformation result (*this);

		result.rotation = rotation.conjugate();
		result.translation = result.rotation.rotate(translation) * -1.f;

		return result;
	}

	Vector3f apply(const Vector3f &vec) const {
		Vector3f temp = rotation.rotate (vec + translation);
		return Vector3f (temp[0] * scaling[0], temp[1] * scaling[1], temp[2] * scaling[2]);
	}
};

/* _TRANSFORMATION_H */
#endif
