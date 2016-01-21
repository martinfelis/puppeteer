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

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "GL/glew.h"

#include <QGLWidget>
#include <QGLFramebufferObject>
#include <QImage>

#include <iostream>
#include <SimpleMath/SimpleMath.h>
#include "Camera.h"
#include "Scene.h"

class GLWidget : public QGLWidget
{
	Q_OBJECT

	public:
		GLWidget(QWidget *parent = 0);
		virtual ~GLWidget();

		QSize minimumSizeHint() const;
		QSize sizeHint() const;
		Vector3f getCameraPoi() { return camera.poi; }
		void setCameraPoi (const Vector3f &poi) { camera.poi = poi; }
		Vector3f getCameraEye() { return camera.eye; }
		void setCameraEye (const Vector3f &eye) { camera.eye = eye; }

		void setScene (Scene* scene_ptr) { scene = scene_ptr; };
		QImage renderContentOffscreen (int image_width, int image_height, bool use_alpha);

		bool draw_base_axes;
		bool draw_grid;

		Camera camera;
		Scene* scene;

		QGLFramebufferObject *colorPickingFrameBuffer;

	protected:
		void drawGrid();

		void initializeGL();
		void drawScene ();
		void paintGL();
		void paintColorPickingFrameBuffer();
		void resizeGL(int width, int height);

		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);

	private:
		QPoint mousePressPos;
		QPoint lastMousePos;

		float windowWidth;
		float windowHeight;

		bool opengl_initialized;

	public slots:
		void toggle_draw_grid(bool status);
		void toggle_draw_base_axes(bool status);
		void toggle_draw_orthographic(bool status);

		void set_front_view ();
		void set_side_view ();
		void set_top_view ();
	
	signals:
		void camera_changed();
		void object_selected(int object_id);
		void object_unselected(int object_id);
};

#endif
