/*
 * QtGLAppBase - Simple Qt Application to get started with OpenGL stuff.
 *
 * Copyright (c) 2011-2012 Martin Felis <martin.felis@iwr.uni-heidelberg.de>
 *
 * Licensed under the MIT license. See LICENSE for more details.
 */

#ifndef _CAMERA_H
#define _CAMERA_H

#include <SimpleMath/SimpleMath.h>

struct Camera {
	Vector3f poi;
	Vector3f eye;
	Vector3f up;

	float phi;
	float theta;
	float r;
	float fov;

	bool orthographic;

	Camera();
	void update(int width, int height);
	void updateSphericalCoordinates();

	void setFrontView();
	void setSideView();
	void setTopView();

	void move (float screen_x, float screen_y);
	void zoom (float screen_y);
	void rotate (float screen_x, float screen_y);
};

/* _CAMERA_H */
#endif
