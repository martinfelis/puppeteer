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

	dockModelStateEditor->setEnabled(false);
	dockWidgetSlider->setVisible(false);

	connect (actionFrontView, SIGNAL (triggered()), glWidget, SLOT (set_front_view()));
	connect (actionSideView, SIGNAL (triggered()), glWidget, SLOT (set_side_view()));
	connect (actionTopView, SIGNAL (triggered()), glWidget, SLOT (set_top_view()));
	connect (actionToggleOrthographic, SIGNAL (toggled(bool)), glWidget, SLOT (toggle_draw_orthographic(bool)));

	// actionQuit() makes sure to set the settings before we quit
	connect (actionQuit, SIGNAL( triggered() ), this, SLOT( quitApplication() ));

	// call the drawing function
	connect (drawTimer, SIGNAL(timeout()), glWidget, SLOT (updateGL()));

	// object selection
	connect (glWidget, SIGNAL(object_selected(int)), this, SLOT (updateWidgetsFromObject(int)));
	connect (glWidget, SIGNAL(object_selected(int)), this, SLOT (updateModelStateEditor(int)));

	// property browser: managers
	doubleReadOnlyManager = new QtDoublePropertyManager(propertiesBrowser);
	stringManager = new QtStringPropertyManager(propertiesBrowser);
	colorManager = new QtColorPropertyManager(propertiesBrowser);
	groupManager = new QtGroupPropertyManager(propertiesBrowser);
	vector3DPropertyManager = new QtVector3DPropertyManager (propertiesBrowser);	
	vector3DReadOnlyPropertyManager = new QtVector3DPropertyManager (propertiesBrowser);	
	vector3DYXZPropertyManager = new QtVector3DPropertyManager (propertiesBrowser);	
	vector3DYXZReadOnlyPropertyManager = new QtVector3DPropertyManager (propertiesBrowser);
	vector3DYXZPropertyManager->setPropertyLabels ("Y", "X", "Z");

	// property browser: editor factories
	doubleSpinBoxFactory = new QtDoubleSpinBoxFactory(propertiesBrowser);
	lineEditFactory = new QtLineEditFactory(propertiesBrowser);
	colorEditFactory = new QtColorEditorFactory(propertiesBrowser);
	vector3DEditorFactory = new QtVector3DEditorFactory(propertiesBrowser);
	vector3DYXZEditorFactory = new QtVector3DEditorFactory(propertiesBrowser);

	// property browser: manager <-> editor
	propertiesBrowser->setFactoryForManager (stringManager, lineEditFactory);
	propertiesBrowser->setFactoryForManager (colorManager, colorEditFactory);
	propertiesBrowser->setFactoryForManager (vector3DPropertyManager, vector3DEditorFactory);
	propertiesBrowser->setFactoryForManager (vector3DPropertyManager->subDoublePropertyManager(), doubleSpinBoxFactory);
	propertiesBrowser->setFactoryForManager (vector3DYXZPropertyManager, vector3DEditorFactory);
	propertiesBrowser->setFactoryForManager (vector3DYXZPropertyManager->subDoublePropertyManager(), doubleSpinBoxFactory);

	// model state editor
	doubleManagerModelStateEditor = new QtDoublePropertyManager(modelStateEditor);
	doubleSpinBoxFactoryModelStateEditor = new QtDoubleSpinBoxFactory(modelStateEditor);
	modelStateEditor->setFactoryForManager (doubleManagerModelStateEditor, doubleSpinBoxFactoryModelStateEditor);

	// signals
	connect (doubleManagerModelStateEditor, SIGNAL (valueChanged(QtProperty *, double)), this, SLOT (modelStateValueChanged (QtProperty *, double)));
	connect (vector3DPropertyManager, SIGNAL (valueChanged(QtProperty *, QVector3D)), this, SLOT (valueChanged (QtProperty *, QVector3D)));
	connect (vector3DYXZPropertyManager, SIGNAL (valueChanged(QtProperty *, QVector3D)), this, SLOT (valueChanged (QtProperty *, QVector3D)));
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

void QtGLBaseApp::updateModelStateEditor (int object_id) {
	if (object_id < 0) {
		modelStateEditor->setEnabled(false);
		return;
	}

	assert (markerModel);

	modelStateEditor->setEnabled(true);

	modelStateEditor->clear();

	VectorNd model_state = markerModel->getModelState();
	vector<string> state_names = markerModel->getModelStateNames();

	assert (model_state.size() == state_names.size());

	for (size_t i = 0; i < model_state.size(); i++) {
		QtProperty *dof_property = doubleManagerModelStateEditor->addProperty (state_names[i].c_str());
		doubleManagerModelStateEditor->setValue (dof_property, model_state[i]);
		doubleManagerModelStateEditor->setSingleStep (dof_property, 0.01);
		modelStateEditor->addProperty (dof_property);
		propertyToStateIndex[dof_property] = i;
	}
}

void QtGLBaseApp::updatePropertiesForFrame (unsigned int frame_id) {
	QtBrowserItem *item = NULL;

	// property browser: properties
	QtProperty *frame_parent_property = stringManager->addProperty ("Parent");
	stringManager->setValue (frame_parent_property, markerModel->getParentName (frame_id).c_str());
	propertiesBrowser->insertProperty (frame_parent_property, 0);

	QtProperty *frame_name_property = stringManager->addProperty ("Name");
	stringManager->setValue (frame_name_property, markerModel->getFrameName (frame_id).c_str());
	propertiesBrowser->insertProperty (frame_name_property, 0);

	// joints
	QtProperty *joint_group = groupManager->addProperty("Joint Frame");

	// joint local position
	QtProperty *joint_location_local_property = vector3DPropertyManager->addProperty("Position");
	Vector3f joint_location_local = markerModel->getJointLocationLocal (frame_id);
	vector3DPropertyManager->setValue (joint_location_local_property, QVector3D (joint_location_local[0], joint_location_local[1], joint_location_local[2]));
	vector3DPropertyManager->setSingleStep (joint_location_local_property, 0.01);
	registerProperty (joint_location_local_property, "joint_location_local");
	joint_group->addSubProperty (joint_location_local_property);

	// joint local orientation
	QtProperty *joint_orientation_local_property = vector3DYXZPropertyManager->addProperty("Orientation");
	Vector3f joint_orientation_local = markerModel->getJointOrientationLocalEulerYXZ (frame_id);
	vector3DYXZPropertyManager->setValue (joint_orientation_local_property, QVector3D (joint_orientation_local[0], joint_orientation_local[1], joint_orientation_local[2]));
	vector3DYXZPropertyManager->setSingleStep (joint_orientation_local_property, 0.01);
	registerProperty (joint_orientation_local_property, "joint_orientation_local");
	joint_group->addSubProperty (joint_orientation_local_property);

	item = propertiesBrowser->addProperty (joint_group);
//	propertiesBrowser->setExpanded (item, false);

	if (item->children().size() > 0) {
		for (QList<QtBrowserItem*>::const_iterator iter = item->children().constBegin(); iter != item->children().constEnd(); iter++) {
			propertiesBrowser->setExpanded ((*iter), false);
		}
	}

//	QtProperty *visuals_group = groupManager->addProperty("Visuals");
//	propertiesBrowser->addProperty (visuals_group);

	restoreExpandStateRecursive(propertiesBrowser->topLevelItems(), "");
}

void QtGLBaseApp::updateWidgetsFromObject (int object_id) {
	if (object_id < 0) {
		propertiesBrowser->clear();
		return;
	}

	Vector3f zyx_rotation = scene->getObject<SceneObject>(object_id)->transformation.rotation.toEulerZYX();

	updateExpandStateRecursive(propertiesBrowser->topLevelItems(), "");

	// update properties browser
	propertiesBrowser->clear();
	QtBrowserItem* item = NULL;

	// global position
	QtProperty *position_property = vector3DPropertyManager->addProperty("Position");
	Vector3f position = scene->getObject<SceneObject>(object_id)->transformation.translation;
	vector3DPropertyManager->setValue (position_property, QVector3D (position[0], position[1], position[2]));
	registerProperty (position_property, "object_position");
	item = propertiesBrowser->addProperty (position_property);
	propertiesBrowser->setExpanded (item, false);

	// global orientation
	QtProperty *orientation_property = vector3DYXZPropertyManager->addProperty("Orientation");
	Vector3f yxz_rotation = scene->getObject<SceneObject>(object_id)->transformation.rotation.toEulerYXZ();
	vector3DYXZPropertyManager->setValue (orientation_property, QVector3D (yxz_rotation[0], yxz_rotation[1], yxz_rotation[2]));
	registerProperty (orientation_property, "object_orientation");
	item = propertiesBrowser->addProperty (orientation_property);
	propertiesBrowser->setExpanded (item, false);

	if (markerModel && markerModel->isModelObject(object_id)) {
		dockModelStateEditor->setEnabled(true);
		unsigned int frame_id = markerModel->getFrameIdFromObjectId (object_id);
		updatePropertiesForFrame (frame_id);
		return;
	} 
	restoreExpandStateRecursive(propertiesBrowser->topLevelItems(), "");
}

void QtGLBaseApp::modelStateValueChanged (QtProperty *property, double value) {
	assert (markerModel);

	if (!propertyToStateIndex.contains(property)) {
		return;
	}

	unsigned int state_index = propertyToStateIndex[property];

	markerModel->setModelStateValue (state_index, value);	
	updateWidgetsFromObject (scene->selectedObjectId);
}

void QtGLBaseApp::valueChanged (QtProperty *property, QVector3D value) {
	if (!propertyToName.contains(property))
		return;

	QString property_name = propertyToName[property];

	if (property_name.startsWith ("object_position")) {
		Vector3f position (value.x(), value.y(), value.z());
		scene->getObject<SceneObject>(scene->selectedObjectId)->transformation.translation = position;
	} else if (property_name.startsWith ("object_orientation")) {
		Vector3f yxz_rotation (value.x(), value.y(), value.z());
		Quaternion rotation = Quaternion::fromEulerYXZ (yxz_rotation);
		scene->getObject<SceneObject>(scene->selectedObjectId)->transformation.rotation = rotation;
	} else if (property_name.startsWith ("joint_location_local")) {
		Vector3f position (value.x(), value.y(), value.z());
		unsigned int frame_id = markerModel->getFrameIdFromObjectId (scene->selectedObjectId);
		markerModel->setJointLocationLocal (frame_id, position);
	} else if (property_name.startsWith ("joint_orientation_local")) {
		Vector3f yxz_rotation (value.x(), value.y(), value.z());
		Quaternion rotation = Quaternion::fromEulerYXZ (yxz_rotation);
		unsigned int frame_id = markerModel->getFrameIdFromObjectId (scene->selectedObjectId);
		markerModel->setJointOrientationLocalEulerYXZ (frame_id, rotation.toEulerYXZ());
	} else {
		qDebug() << "Warning! Unhandled value change of property " << property_name;
	}
}

void QtGLBaseApp::captureFrameSliderChanged (int value) {
	assert (markerData);
	markerData->setCurrentFrameNumber (value);
}
