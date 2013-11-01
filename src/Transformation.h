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
