#ifndef _TRANSFORMATION_H
#define _TRANSFORMATION_H

#include "SimpleMath/SimpleMath.h"
#include "SimpleMath/SimpleMathGL.h"

#include <iostream>

struct Transformation {
	Transformation() : 
		translation (0.f, 0.f, 0.f),
		rotation (0.f, 0.f, 0.f, 1.f) {}
	Transformation ( const Vector3f &translation, const SimpleMath::GL::Quaternion& rotation = SimpleMath::GL::Quaternion(0., 0., 0., 1.)) :
		translation (translation),
		rotation (rotation) {}
	Transformation ( const Vector3f &translation, const Matrix33f rotation_mat) :
		translation (translation),
		rotation (SimpleMath::GL::Quaternion::fromMatrix(rotation_mat)) {}

	Vector3f translation;
	SimpleMath::GL::Quaternion rotation;

	Matrix44f toGLMatrix() const {
		return rotation.toGLMatrix() * SimpleMath::GL::TranslateMat44 (translation[0], translation[1], translation[2]);
	}
	Transformation operator* (const Transformation &transform) const {
		Transformation result (*this);

		result.translation = transform.rotation.rotate(translation) + transform.translation ;
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
		return rotation.rotate(vec) + translation;
	}
};

/* _TRANSFORMATION_H */
#endif
