/*
 * QtGLAppBase - Simple Qt Application to get started with OpenGL stuff.
 *
 * Copyright (c) 2011-2012 Martin Felis <martin.felis@iwr.uni-heidelberg.de>
 *
 * Licensed under the MIT license. See LICENSE for more details.
 */

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "GL/glew.h"

#include <QGLWidget>
#include <QGLFramebufferObject>

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

		void keyPressEvent (QKeyEvent* event);
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
