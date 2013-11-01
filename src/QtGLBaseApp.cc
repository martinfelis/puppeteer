/*
 * QtGLAppBase - Simple Qt Application to get started with OpenGL stuff.
 *
 * Copyright (c) 2011-2012 Martin Felis <martin.felis@iwr.uni-heidelberg.de>
 *
 * Licensed under the MIT license. See LICENSE for more details.
 */

#include <QtGui> 
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QProgressDialog>
#include <QRegExp>
#include <QRegExpValidator>

#include "GLWidget.h" 
#include "QtGLBaseApp.h"
#include "Scene.h"

#include <sys/time.h>
#include <ctime>

#include <assert.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <fstream>

using namespace std;

using namespace SimpleMath::GL;

const double TimeLineDuration = 1000.;

std::string vec3_to_string (const Vector3f vec3, unsigned int digits = 2) {
	stringstream vec_stream ("");
	vec_stream << std::fixed << std::setprecision(digits) << vec3[0] << ", " << vec3[1] << ", " << vec3[2];

	return vec_stream.str();
}

Vector3f string_to_vec3 (const std::string &vec3_string) {
	Vector3f result;

	unsigned int token_start = 0;
	unsigned int token_end = vec3_string.find(",");
	for (unsigned int i = 0; i < 3; i++) {
		string token = vec3_string.substr (token_start, token_end - token_start);

		result[i] = static_cast<float>(atof(token.c_str()));

		token_start = token_end + 1;
		token_end = vec3_string.find (", ", token_start);
	}

	return result;
}

QtGLBaseApp::~QtGLBaseApp() {
	if (scene)
		delete scene;

	scene = NULL;
}

QtGLBaseApp::QtGLBaseApp(QWidget *parent)
{
	setupUi(this); // this sets up GUI

	// create Scene
	scene = new Scene;	
	scene->init();
	glWidget->setScene (scene);

	draw_timer = new QTimer (this);
	draw_timer->setSingleShot(false);
	draw_timer->start(20);

	checkBoxDrawBaseAxes->setChecked (glWidget->draw_base_axes);
	checkBoxDrawGrid->setChecked (glWidget->draw_grid);

	// camera controls
	QRegExp	vector3_expr ("^\\s*-?\\d*(\\.|\\.\\d+)?\\s*,\\s*-?\\d*(\\.|\\.\\d+)?\\s*,\\s*-?\\d*(\\.|\\.\\d+)?\\s*$");
	QRegExpValidator *coord_validator_eye = new QRegExpValidator (vector3_expr, lineEditCameraEye);
	QRegExpValidator *coord_validator_center = new QRegExpValidator (vector3_expr, lineEditCameraCenter);
	lineEditCameraEye->setValidator (coord_validator_eye);
	lineEditCameraCenter->setValidator (coord_validator_center);

	dockCameraControls->setVisible(false);

	// view stettings
	connect (checkBoxDrawBaseAxes, SIGNAL (toggled(bool)), glWidget, SLOT (toggle_draw_base_axes(bool)));
	connect (checkBoxDrawGrid, SIGNAL (toggled(bool)), glWidget, SLOT (toggle_draw_grid(bool)));
	//connect (checkBoxDrawShadows, SIGNAL (toggled(bool)), glWidget, SLOT (toggle_draw_shadows(bool)));

	connect (actionFrontView, SIGNAL (triggered()), glWidget, SLOT (set_front_view()));
	connect (actionSideView, SIGNAL (triggered()), glWidget, SLOT (set_side_view()));
	connect (actionTopView, SIGNAL (triggered()), glWidget, SLOT (set_top_view()));
	connect (actionToggleOrthographic, SIGNAL (toggled(bool)), glWidget, SLOT (toggle_draw_orthographic(bool)));

	// action_quit() makes sure to set the settings before we quit
	connect (actionQuit, SIGNAL( triggered() ), this, SLOT( action_quit() ));
	connect (pushButtonUpdateCamera, SIGNAL (clicked()), this, SLOT (update_camera()));

	// call the drawing function
	connect (draw_timer, SIGNAL(timeout()), glWidget, SLOT (updateGL()));

	// object selection
	connect (glWidget, SIGNAL(object_selected(int)), this, SLOT (updateWidgetsFromObject(int)));

	// object widgets
	QRegExpValidator *position_validator = new QRegExpValidator (vector3_expr, lineEditObjectPosition);
	lineEditObjectPosition->setValidator(position_validator);
	connect (lineEditObjectPosition, SIGNAL(editingFinished()), this, SLOT(updateObjectFromWidget()));

	QRegExpValidator *rotation_validator = new QRegExpValidator (vector3_expr, lineEditObjectRotation);
	lineEditObjectRotation->setValidator(rotation_validator);
	connect (lineEditObjectRotation, SIGNAL(editingFinished()), this, SLOT(updateObjectFromWidget()));

	QRegExpValidator *scale_validator = new QRegExpValidator (vector3_expr, lineEditObjectScale);
	lineEditObjectScale->setValidator(scale_validator);
	connect (lineEditObjectScale, SIGNAL(editingFinished()), this, SLOT(updateObjectFromWidget()));
}

void print_usage() {
	cout << "Usage: meshup [model_name] [animation_file] " << endl
		<< "Visualization tool for multi-body systems based on skeletal animation and magic." << endl
		<< endl
		<< "Report bugs to martin.felis@iwr.uni-heidelberg.de" << endl;
}

void QtGLBaseApp::camera_changed() {
	Vector3f center = glWidget->getCameraPoi();	
	Vector3f eye = glWidget->getCameraEye();	

	unsigned int digits = 2;

	stringstream center_stream ("");
	center_stream << std::fixed << std::setprecision(digits) << center[0] << ", " << center[1] << ", " << center[2];

	stringstream eye_stream ("");
	eye_stream << std::fixed << std::setprecision(digits) << eye[0] << ", " << eye[1] << ", " << eye[2];

	lineEditCameraEye->setText (eye_stream.str().c_str());
	lineEditCameraCenter->setText (center_stream.str().c_str());
}

Vector3f parse_vec3_string (const std::string vec3_string) {
	Vector3f result;

	unsigned int token_start = 0;
	unsigned int token_end = vec3_string.find(",");
	for (unsigned int i = 0; i < 3; i++) {
		string token = vec3_string.substr (token_start, token_end - token_start);

		result[i] = static_cast<float>(atof(token.c_str()));

		token_start = token_end + 1;
		token_end = vec3_string.find (", ", token_start);
	}

//	cout << "Parsed '" << vec3_string << "' to " << result.transpose() << endl;

	return result;
}

void QtGLBaseApp::update_camera() {
	string center_string = lineEditCameraCenter->text().toStdString();
	Vector3f poi = parse_vec3_string (center_string);

	string eye_string = lineEditCameraEye->text().toStdString();
	Vector3f eye = parse_vec3_string (eye_string);

	glWidget->setCameraPoi(poi);
	glWidget->setCameraEye(eye);
}

void QtGLBaseApp::action_quit () {
	qApp->quit();
}

void QtGLBaseApp::updateWidgetsFromObject (int object_id) {
	if (object_id < 0) {
		lineEditObjectPosition->setText ("");

		return;
	}

	lineEditObjectPosition->setText (vec3_to_string (scene->objects[object_id].transformation.translation).c_str());

	Vector3f zyx_rotation = scene->objects[object_id].transformation.rotation.toEulerZYX() * 180.f / static_cast<float>(M_PI);

	lineEditObjectRotation->setText (vec3_to_string (zyx_rotation).c_str());

}

void QtGLBaseApp::updateObjectFromWidget () {
	if (scene->selectedObjectId < 0)
		return;

	Vector3f position = string_to_vec3 (lineEditObjectPosition->text().toStdString());

	Vector3f zyx_rotation = string_to_vec3 (lineEditObjectRotation->text().toStdString()) * M_PI / 180.f;
	Quaternion rotation = Quaternion::fromEulerZYX (zyx_rotation);

	scene->objects[scene->selectedObjectId].transformation.translation = position;
	scene->objects[scene->selectedObjectId].transformation.rotation = rotation;
}

