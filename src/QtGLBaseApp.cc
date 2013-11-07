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
#include "QtVariantPropertyManager"

#include "Scene.h"
#include "MarkerModel.h"
#include "MarkerData.h"

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

	// marker model and data
	markerModel = NULL;
	markerData = NULL;

	drawTimer = new QTimer (this);
	drawTimer->setSingleShot(false);
	drawTimer->start(20);

	dockCameraControls->setVisible(false);
	dockRenderSettings->setVisible(false);
	dockWidgetSlider->setVisible(false);

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

	// actionQuit() makes sure to set the settings before we quit
	connect (actionQuit, SIGNAL( triggered() ), this, SLOT( quitApplication() ));
	connect (pushButtonUpdateCamera, SIGNAL (clicked()), this, SLOT (updateCamera()));

	// call the drawing function
	connect (drawTimer, SIGNAL(timeout()), glWidget, SLOT (updateGL()));

	// object selection
	connect (glWidget, SIGNAL(object_selected(int)), this, SLOT (updateWidgetsFromObject(int)));

	// property browser: managers
	doubleReadOnlyManager = new QtDoublePropertyManager(propertiesBrowser);
	doubleManager = new QtDoublePropertyManager(propertiesBrowser);
	stringManager = new QtStringPropertyManager(propertiesBrowser);
	colorManager = new QtColorPropertyManager(propertiesBrowser);
	groupManager = new QtGroupPropertyManager(propertiesBrowser);
	vector3DPropertyManager = new QtVector3DPropertyManager (propertiesBrowser);	

	// property browser: editor factories
	doubleSpinBoxFactory = new QtDoubleSpinBoxFactory(propertiesBrowser);
	lineEditFactory = new QtLineEditFactory(propertiesBrowser);
	colorEditFactory = new QtColorEditorFactory(propertiesBrowser);
	vector3DEditorFactory = new QtVector3DEditorFactory(propertiesBrowser);

	// property browser: manager <-> editor
	propertiesBrowser->setFactoryForManager (doubleManager, doubleSpinBoxFactory);
	propertiesBrowser->setFactoryForManager (stringManager, lineEditFactory);
	propertiesBrowser->setFactoryForManager (colorManager, colorEditFactory);
	propertiesBrowser->setFactoryForManager (vector3DPropertyManager, vector3DEditorFactory);
	propertiesBrowser->setFactoryForManager (vector3DPropertyManager->subDoublePropertyManager(), doubleSpinBoxFactory);

	// signals
	connect (doubleManager, SIGNAL (valueChanged(QtProperty *, double)), this, SLOT (valueChanged (QtProperty *, double)));
}

void print_usage(const char* execname) {
	cout << "Usage: " << execname << " <modelfile.lua> <mocapdata.c3d>" << endl;
}

bool QtGLBaseApp::parseArgs(int argc, char* argv[]) {
	if (argc == 1)
		return true;

	for (int i = 1; i < argc; i++) {
		std::string arg (argv[i]);
		if (arg.substr(arg.size() - 4, 4) == ".lua")
			loadModelFile (arg.c_str());
		else if (arg.substr(arg.size() - 4, 4) == ".c3d")
			loadMocapFile (arg.c_str());
		else {
			print_usage (argv[0]);
			return false;
		}
	}

	return true;
}

bool QtGLBaseApp::loadModelFile (const char* filename) {
	if (markerModel)
		delete markerModel;
	markerModel = new MarkerModel (scene);
	assert (markerModel);
	return markerModel->loadFromFile (filename);
}

bool QtGLBaseApp::loadMocapFile (const char* filename) {
	if (markerData)
		delete markerData;
	markerData = new MarkerData (scene);
	assert (markerData);

	if(!markerData->loadFromFile (filename))
		return false;

	dockWidgetSlider->setVisible(true);
	captureFrameSlider->setMinimum (markerData->getFirstFrame());
	captureFrameSlider->setMaximum (markerData->getLastFrame());

	connect (captureFrameSlider, SIGNAL (valueChanged(int)), this, SLOT (captureFrameSliderChanged (int)));

	return true;
}

void QtGLBaseApp::cameraChanged() {
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

void QtGLBaseApp::updateCamera() {
	string center_string = lineEditCameraCenter->text().toStdString();
	Vector3f poi = parse_vec3_string (center_string);

	string eye_string = lineEditCameraEye->text().toStdString();
	Vector3f eye = parse_vec3_string (eye_string);

	glWidget->setCameraPoi(poi);
	glWidget->setCameraEye(eye);
}

void QtGLBaseApp::quitApplication() {
	qApp->quit();
}

void QtGLBaseApp::collapseProperties() {
	// collapse all items
	QList<QtBrowserItem*>::ConstIterator prop_iter = propertiesBrowser->topLevelItems().constBegin();

	while (prop_iter != propertiesBrowser->topLevelItems().constEnd()) {
		propertiesBrowser->setExpanded (*prop_iter, false);
		prop_iter++;
	}
}

void QtGLBaseApp::updateExpandStateRecursive (const QList<QtBrowserItem *> &list, const QString &parent_property_id) {
	QListIterator<QtBrowserItem *> it(list);
	while (it.hasNext()) {
		QtBrowserItem *item = it.next();
		QtProperty *prop = item->property();
		QString property_id = parent_property_id + prop->propertyName();

		if (item->children().size() > 0) {
			updateExpandStateRecursive (item->children(), property_id);
		}

		idToExpanded[property_id] = propertiesBrowser->isExpanded(item);
	}
}

void QtGLBaseApp::restoreExpandStateRecursive (const QList<QtBrowserItem *> &list, const QString &parent_property_id) {
	QListIterator<QtBrowserItem *> it(list);
	while (it.hasNext()) {
		QtBrowserItem *item = it.next();
		QtProperty *prop = item->property();
		QString property_id = parent_property_id + prop->propertyName();

		if (item->children().size() > 0) {
			restoreExpandStateRecursive (item->children(), property_id);
		}

		if ( idToExpanded.contains(property_id))
			propertiesBrowser->setExpanded(item, idToExpanded[property_id]);
	}
}

void QtGLBaseApp::updatePropertiesForFrame (unsigned int frame_id) {

	updateExpandStateRecursive(propertiesBrowser->topLevelItems(), "");

	// update properties browser
	propertiesBrowser->clear();

	// property browser: properties
	QtProperty *frame_name_property = stringManager->addProperty ("Name");
	stringManager->setValue (frame_name_property, markerModel->getFrameName (frame_id).c_str());
	propertiesBrowser->addProperty (frame_name_property);

	// Test Vector3d property
	QtProperty *position_property = vector3DPropertyManager->addProperty("Position");
	vector3DPropertyManager->setValue (position_property, QVector3D (1., 2., 3.));
	propertiesBrowser->addProperty (position_property);

	// world position
	QtProperty *world_position_group = groupManager->addProperty("Position");
	Vector3f frame_position_global = markerModel->getFrameLocationGlobal (frame_id);
	QtProperty *world_position_x = doubleReadOnlyManager->addProperty("X");
	QtProperty *world_position_y = doubleReadOnlyManager->addProperty("Y");
	QtProperty *world_position_z = doubleReadOnlyManager->addProperty("Z");
	doubleReadOnlyManager->setValue (world_position_x, frame_position_global[0]);
	doubleReadOnlyManager->setValue (world_position_y, frame_position_global[1]);
	doubleReadOnlyManager->setValue (world_position_z, frame_position_global[2]);
	world_position_group->addSubProperty (world_position_x);
	world_position_group->addSubProperty (world_position_y);
	world_position_group->addSubProperty (world_position_z);
	propertiesBrowser->addProperty (world_position_group);

	// world orientation
	QtProperty *world_orientation_group = groupManager->addProperty("Orientation");
	Vector3f frame_orientation_global = markerModel->getFrameOrientationGlobalEulerYXZ (frame_id);
	QtProperty *world_orientation_y = doubleReadOnlyManager->addProperty("Y");
	QtProperty *world_orientation_x = doubleReadOnlyManager->addProperty("X");
	QtProperty *world_orientation_z = doubleReadOnlyManager->addProperty("Z");
	doubleReadOnlyManager->setValue (world_orientation_y, frame_orientation_global[0]);
	doubleReadOnlyManager->setValue (world_orientation_x, frame_orientation_global[1]);
	doubleReadOnlyManager->setValue (world_orientation_z, frame_orientation_global[2]);
	world_orientation_group->addSubProperty (world_orientation_y);
	world_orientation_group->addSubProperty (world_orientation_x);
	world_orientation_group->addSubProperty (world_orientation_z);
	propertiesBrowser->addProperty (world_orientation_group);

	// joints
	QtProperty *joint_group = groupManager->addProperty("Joint");

	// joint local position
	QtProperty *joint_position_group = groupManager->addProperty("Position");
	Vector3f joint_location_local = markerModel->getJointLocationLocal (frame_id);
	QtProperty *joint_position_x = doubleManager->addProperty("X");
	QtProperty *joint_position_y = doubleManager->addProperty("Y");
	QtProperty *joint_position_z = doubleManager->addProperty("Z");
	registerProperty (joint_position_x, "joint_local_position_x");
	registerProperty (joint_position_y, "joint_local_position_y");
	registerProperty (joint_position_z, "joint_local_position_z");
	doubleManager->setValue (joint_position_x, joint_location_local[0]);
	doubleManager->setValue (joint_position_y, joint_location_local[1]);
	doubleManager->setValue (joint_position_z, joint_location_local[2]);
	joint_position_group->addSubProperty (joint_position_x);
	joint_position_group->addSubProperty (joint_position_y);
	joint_position_group->addSubProperty (joint_position_z);
	joint_group->addSubProperty (joint_position_group);	

	// joint local orientation
	QtProperty *joint_orientation_group = groupManager->addProperty("Orientation");
	Vector3f joint_orientation_local = markerModel->getJointOrientationLocalEulerYXZ (frame_id);
	QtProperty *joint_orientation_y = doubleManager->addProperty("Y");
	QtProperty *joint_orientation_x = doubleManager->addProperty("X");
	QtProperty *joint_orientation_z = doubleManager->addProperty("Z");
	registerProperty (joint_orientation_x, "joint_local_orientation_x");
	registerProperty (joint_orientation_y, "joint_local_orientation_y");
	registerProperty (joint_orientation_z, "joint_local_orientation_z");
	doubleManager->setValue (joint_orientation_y, joint_orientation_local[0]);
	doubleManager->setValue (joint_orientation_x, joint_orientation_local[1]);
	doubleManager->setValue (joint_orientation_z, joint_orientation_local[2]);
	joint_orientation_group->addSubProperty (joint_orientation_y);
	joint_orientation_group->addSubProperty (joint_orientation_x);
	joint_orientation_group->addSubProperty (joint_orientation_z);
	joint_group->addSubProperty (joint_orientation_group);	

	propertiesBrowser->addProperty (joint_group);


	/*
	// joints: position
	Vector3f position = scene->getObject(object_id).transformation.translation;

	QtProperty *joint_position_group = groupManager->addProperty("Position");
	QtProperty *position_x = doubleManager->addProperty("X");
	QtProperty *position_y = doubleManager->addProperty("Y");
	QtProperty *position_z = doubleManager->addProperty("Z");

	joint_position_group->addSubProperty (position_x);
	joint_position_group->addSubProperty (position_y);
	joint_position_group->addSubProperty (position_z);

	registerProperty (position_x, "joint_position_x");
	registerProperty (position_y, "joint_position_y");
	registerProperty (position_z, "joint_position_z");

	doubleManager->setValue (position_x, position[0]);
	doubleManager->setValue (position_y, position[1]);
	doubleManager->setValue (position_z, position[2]);

	joint_group->addSubProperty (joint_position_group);

	// joints: orientation
	QtProperty *joint_orientation_group = groupManager->addProperty("Orientation");
	QtProperty *orientation_x = doubleManager->addProperty("X");
	QtProperty *orientation_y = doubleManager->addProperty("Y");
	QtProperty *orientation_z = doubleManager->addProperty("Z");

	joint_orientation_group->addSubProperty (orientation_y);
	joint_orientation_group->addSubProperty (orientation_x);
	joint_orientation_group->addSubProperty (orientation_z);

	registerProperty (orientation_x, "joint_orientation_x");
	registerProperty (orientation_y, "joint_orientation_y");
	registerProperty (orientation_z, "joint_orientation_z");

	Vector3f yxz_rotation = scene->getObject(object_id).transformation.rotation.toEulerYXZ() * 180.f / static_cast<float>(M_PI);
	doubleManager->setValue(orientation_y, yxz_rotation[0]);
	doubleManager->setValue(orientation_x, yxz_rotation[1]);
	doubleManager->setValue(orientation_z, yxz_rotation[2]);

	joint_group->addSubProperty (joint_orientation_group);

	QtProperty *visuals_group = groupManager->addProperty("Visuals");

	propertiesBrowser->addProperty (joint_group);
	propertiesBrowser->addProperty (visuals_group);
*/
	restoreExpandStateRecursive(propertiesBrowser->topLevelItems(), "");
}

void QtGLBaseApp::updateWidgetsFromObject (int object_id) {
	if (object_id < 0) {
		return;
	}

	if (markerModel && markerModel->isModelObject(object_id)) {
		unsigned int frame_id = markerModel->getFrameIdFromObjectId (object_id);
		qDebug() << "clicked on joint of frame " << frame_id << "!";
		updatePropertiesForFrame (frame_id);
		return;
	}

	Vector3f zyx_rotation = scene->getObject(object_id).transformation.rotation.toEulerZYX() * 180.f / static_cast<float>(M_PI);

	updateExpandStateRecursive(propertiesBrowser->topLevelItems(), "");

	// update properties browser
	propertiesBrowser->clear();

	// property browser: properties
	QtProperty *frame_name_property = stringManager->addProperty ("Name");

	// world position and orientation

	// joints
	QtProperty *joint_group = groupManager->addProperty("Joint");

	// joints: position
	Vector3f position = scene->getObject(object_id).transformation.translation;

	QtProperty *joint_position_group = groupManager->addProperty("Position");
	QtProperty *position_x = doubleManager->addProperty("X");
	QtProperty *position_y = doubleManager->addProperty("Y");
	QtProperty *position_z = doubleManager->addProperty("Z");

	joint_position_group->addSubProperty (position_x);
	joint_position_group->addSubProperty (position_y);
	joint_position_group->addSubProperty (position_z);

	registerProperty (position_x, "joint_position_x");
	registerProperty (position_y, "joint_position_y");
	registerProperty (position_z, "joint_position_z");

	doubleManager->setValue (position_x, position[0]);
	doubleManager->setValue (position_y, position[1]);
	doubleManager->setValue (position_z, position[2]);

	joint_group->addSubProperty (joint_position_group);

	// joints: orientation
	QtProperty *joint_orientation_group = groupManager->addProperty("Orientation");
	QtProperty *orientation_x = doubleManager->addProperty("X");
	QtProperty *orientation_y = doubleManager->addProperty("Y");
	QtProperty *orientation_z = doubleManager->addProperty("Z");

	joint_orientation_group->addSubProperty (orientation_y);
	joint_orientation_group->addSubProperty (orientation_x);
	joint_orientation_group->addSubProperty (orientation_z);

	registerProperty (orientation_x, "joint_orientation_x");
	registerProperty (orientation_y, "joint_orientation_y");
	registerProperty (orientation_z, "joint_orientation_z");

	Vector3f yxz_rotation = scene->getObject(object_id).transformation.rotation.toEulerYXZ() * 180.f / static_cast<float>(M_PI);
	doubleManager->setValue(orientation_y, yxz_rotation[0]);
	doubleManager->setValue(orientation_x, yxz_rotation[1]);
	doubleManager->setValue(orientation_z, yxz_rotation[2]);

	joint_group->addSubProperty (joint_orientation_group);

	QtProperty *visuals_group = groupManager->addProperty("Visuals");

	propertiesBrowser->addProperty (frame_name_property);
	propertiesBrowser->addProperty (joint_group);
	propertiesBrowser->addProperty (visuals_group);

	restoreExpandStateRecursive(propertiesBrowser->topLevelItems(), "");
}

void QtGLBaseApp::valueChanged (QtProperty *property, double value) {
	if (!propertyToName.contains(property))
		return;

	QString property_name = propertyToName[property];

	if (property_name.startsWith ("joint_orientation")) {
		Vector3f yxz_rotation;
		yxz_rotation[0] = doubleManager->value(nameToProperty["joint_orientation_y"]);
		yxz_rotation[1] = doubleManager->value(nameToProperty["joint_orientation_x"]);
		yxz_rotation[2] = doubleManager->value(nameToProperty["joint_orientation_z"]);

		Quaternion rotation = Quaternion::fromEulerYXZ (yxz_rotation * M_PI / 180.f);

		scene->getObject(scene->selectedObjectId).transformation.rotation = rotation;
	} else if (property_name.startsWith ("joint_position")) {
		Vector3f position;
		position[0] = doubleManager->value(nameToProperty["joint_position_x"]);
		position[1] = doubleManager->value(nameToProperty["joint_position_y"]);
		position[2] = doubleManager->value(nameToProperty["joint_position_z"]);

		scene->getObject(scene->selectedObjectId).transformation.translation = position;
	} else {
		qDebug() << "Warning! Unhandled value change of property " << property_name;
	}
}

void QtGLBaseApp::captureFrameSliderChanged (int value) {
	assert (markerData);
	markerData->setCurrentFrameNumber (value);
}
