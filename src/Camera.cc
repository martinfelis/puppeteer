/* 
 * Puppeteer - A Motion Capture Mapping Tool
 * Copyright (c) 2013-2016 Martin Felis <martin.felis@iwr.uni-heidelberg.de>.
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

#include "GL/glew.h"
#include <GL/glu.h>

#include "Camera.h"
#include "SimpleMath/SimpleMathGL.h"

Camera::Camera() :
	poi(Vector3f(0.f, 1.f, 0.f)),
	eye(Vector3f(6.f, 3.f, 6.f)),
	up (Vector3f (0.f, 1.f, 0.f)),
	phi (0.0f),
	theta (0.0f),
	r (3.0f),
	fov (45.0f),
	orthographic(false)
	{
	updateSphericalCoordinates();
}

void Camera::update(int width, int height) {
	// update the camera
	float s_theta, c_theta, s_phi, c_phi;
	s_theta = sin (theta);
	c_theta = cos (theta);
	s_phi = sin (phi);
	c_phi = cos (phi);

	Matrix44f rot_mat_44 = SimpleMath::GL::RotateMat44 (90.f, 1.f, 0.f, 0.f);
	Matrix33f rot_mat = rot_mat_44.block<3,3>(0,0);

	eye[0] = (r * s_theta * c_phi);
	eye[1] = -(r * s_theta * s_phi);
	eye[2] = (r * c_theta);

	eye += poi;

	Vector3f right (-s_phi, -c_phi, 0.f);

	Vector3f eye_normalized (eye);
	eye_normalized.normalize();

	up = right.cross (eye_normalized);

	// setup of the projection
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	GLfloat aspect = (GLfloat) width / (GLfloat) height;

	if (orthographic) {
		GLfloat distance = (GLfloat) (poi - eye).norm();

		GLfloat w = tan(fov * M_PI / 180.) * 0.01 * width * distance / 10.f;
		GLfloat h = w / aspect; 

		glOrtho (- w * 0.5, w * 0.5,
				-h * 0.5, h * 0.5,
			0.005, 200);
	} else {
		gluPerspective (fov, aspect, 0.005, 200);
	}

	// setup of the modelview matrix
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt (eye[0], eye[1], eye[2],
			poi[0], poi[1], poi[2],
			up[0], up[1], up[2]);
}

void Camera::updateSphericalCoordinates() {
	Vector3f los = poi - eye;
	r = los.norm();
	theta = acos (-los[1] / r);
	phi = atan (los[2] / los[0]);

	// atan only returns +- pi/2 so we have to fix the azimuth here
	if (los[0] > 0.f) {
		if (los[2] >= 0.f)
			phi += M_PI;
		else
			phi -= M_PI;
	}
}

void Camera::setFrontView () {
	if (fabs(phi) > 1.0e-5) {
		// front
		theta = 90. * M_PI / 180.;
		phi = 0.;
	}	else {
		// back
		theta = 90. * M_PI / 180.;
		phi = 180 * M_PI / 180.;
	}
}

void Camera::setSideView () {
	if (fabs(fabs(phi) - 90. * M_PI / 180.) > 1.0e-5 && fabs(fabs(theta) - 90. * M_PI / 180.)) {
		// right
		theta = 90. * M_PI / 180.;
		phi = 90. * M_PI / 180.;
	} else {
		// left
		theta = 90. * M_PI / 180.;
		phi = 270. * M_PI / 180.;
	}
}

void Camera::setTopView () {
	if (fabs(theta) > 1.0e-5) {
		// top
		phi = 180. * M_PI / 180.;
		theta = 0.;
	} else {
		// bottom
		phi = 0. * M_PI / 180.;
		theta = 180. * M_PI / 180.;
	}
}

void Camera::setSize (int width_, int height_) {
	width = width_;
	height = height_;
}

void Camera::move (float screen_x, float screen_y) {
	Vector3f eye_normalized (poi - eye);
	eye_normalized.normalize();

	Vector3f global_y (0.f, 1.f, 0.f);
	Vector3f right = up.cross (eye_normalized);
	Vector3f local_up = eye_normalized.cross(right);
	poi += right * screen_x * 0.01f + local_up* screen_y * 0.01f;
	eye += right * screen_x * 0.01f + local_up* screen_y * 0.01f;
}

void Camera::zoom (float screen_y) {
	r += 0.05 * screen_y;
	r = std::max (0.01f, r);
}

void Camera::rotate (float screen_x, float screen_y) {
	phi += 0.01 * screen_x;
	theta -= 0.01 * screen_y;

	theta = std::max(theta, 0.01f);
	theta = std::min(theta, static_cast<float>(M_PI * 0.99));
}

