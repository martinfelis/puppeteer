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

#ifndef _CAMERA_H
#define _CAMERA_H

#include <SimpleMath/SimpleMath.h>

/**
 * \warning This class is hacked such that it uses the following
 * coordinate system: X forward, Y left, Z up. However the vectors in poi,
 * eye, up use the standard OpenGL coordinate system: X forward, Y up, Z
 * right. (Note: left and right are here seen from a person standing at
 * the origin).
 */
struct Camera {
	Vector3f poi;
	Vector3f eye;
	Vector3f up;

	int width;
	int height;

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
	void setSize (int width_, int height_);

	void move (float screen_x, float screen_y);
	void zoom (float screen_y);
	void rotate (float screen_x, float screen_y);
};

/* _CAMERA_H */
#endif
