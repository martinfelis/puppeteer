/*
 * QtGLAppBase - Simple Qt Application to get started with OpenGL stuff.
 *
 * Copyright (c) 2011-2012 Martin Felis <martin.felis@iwr.uni-heidelberg.de>
 *
 * Licensed under the MIT license. See LICENSE for more details.
 */

#ifndef _FRAMECONFIG_H
#define _FRAMECONFIG_H

#include "SimpleMath/SimpleMath.h"
#include "SimpleMath/SimpleMathGL.h"

/** \brief Keeps track of convention used for frames */
struct FrameConfig {
	FrameConfig() :
		axis_front(1.f, 0.f, 0.f),
		axis_up(0.f, 1.f, 0.f),
		axis_right (0.f, 0.f, 1.f),
		axes_rotation (Matrix33f::Identity())
	{ 
		rotation_order[0] = 2;
		rotation_order[1] = 1;
		rotation_order[2] = 0;
	}
	Vector3f axis_front;
	Vector3f axis_up;
	Vector3f axis_right;
	Matrix33f axes_rotation;

	int rotation_order[3];

	void init() {
		axes_rotation(0,0) = axis_front[0];
		axes_rotation(1,0) = axis_front[1];
		axes_rotation(2,0) = axis_front[2];

		axes_rotation(0,1) = axis_up[0];
		axes_rotation(1,1) = axis_up[1];
		axes_rotation(2,1) = axis_up[2];

		axes_rotation(0,2) = axis_right[0];
		axes_rotation(1,2) = axis_right[1];
		axes_rotation(2,2) = axis_right[2];
	}

	Matrix44f convertAnglesToMatrix (const Vector3f &rotation_angles) const {
		return	smRotate (rotation_angles[0], 1.f, 0.f, 0.f)
			* smRotate (rotation_angles[1], 0.f, 1.f, 0.f)
			* smRotate (rotation_angles[2], 0.f, 0.f, 1.f);
	};

	smQuaternion convertAnglesToQuaternion (const Vector3f &rotation_angles) const {
		int a0 = rotation_order[2];
		int a1 = rotation_order[1];
		int a2 = rotation_order[0];

		Vector3f axis_0 (
				axes_rotation(a0, 0),
				axes_rotation(a0, 1),
				axes_rotation(a0, 2)
				);

		Vector3f axis_1 (
				axes_rotation(a1, 0),
				axes_rotation(a1, 1),
				axes_rotation(a1, 2)
				);

		Vector3f axis_2 (
				axes_rotation(a2, 0),
				axes_rotation(a2, 1),
				axes_rotation(a2, 2)
				);

		return smQuaternion::fromGLRotate (
				rotation_angles[a0],
				axis_0[0], axis_0[1], axis_0[2]
				)
			* smQuaternion::fromGLRotate (
					rotation_angles[a1],
					axis_1[0], axis_1[1], axis_1[2]
					)
			* smQuaternion::fromGLRotate (
					rotation_angles[a2],
					axis_2[0], axis_2[1], axis_2[2]
					);
	}

};

/* _FRAMECONFIG_H */
#endif
