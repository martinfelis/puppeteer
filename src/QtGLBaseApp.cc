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
#include <QProgressDialog>

#include "GLWidget.h" 
#include "QtGLBaseApp.h"
#include "QtVariantPropertyManager"

#include "Scene.h"
#include "MarkerModel.h"
#include "MarkerData.h"
#include "ModelFitter.h"
#include "Animation.h"

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
	glWidget->setScene (scene);

	// marker model and data
	markerModel = NULL;
	markerData = NULL;
	modelFitter = NULL;
	animationData = NULL;
	activeModelFrame = 0;
	activeObject = 0;

	drawTimer = new QTimer (this);
	drawTimer->setSingleShot(false);
	drawTimer->start(20);

	dockModelStateEditor->setVisible(false);
	dockWidgetSlider->setVisible(false);
	autoIKButton->setEnabled(false);

	slideAnimationCheckBox->setEnabled(false);
	slideMarkersCheckBox->setEnabled(false);

	connect (actionFrontView, SIGNAL (triggered()), glWidget, SLOT (set_front_view()));
	connect (actionSideView, SIGNAL (triggered()), glWidget, SLOT (set_side_view()));
	connect (actionTopView, SIGNAL (triggered()), glWidget, SLOT (set_top_view()));
	connect (actionToggleOrthographic, SIGNAL (toggled(bool)), glWidget, SLOT (toggle_draw_orthographic(bool)));

	// actionQuit() makes sure to set the settings before we quit
	connect (actionQuit, SIGNAL( triggered() ), this, SLOT( quitApplication() ));

	// call the drawing function
	connect (drawTimer, SIGNAL(timeout()), glWidget, SLOT (updateGL()));

	// object selection
	connect (glWidget, SIGNAL(object_selected(int)), this, SLOT (objectSelected(int)));

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

	connect (saveModelStateButton, SIGNAL (clicked()), this, SLOT (saveModelState()));
	connect (loadModelStateButton, SIGNAL (clicked()), this, SLOT (loadModelState()));
	connect (saveModelButton, SIGNAL (clicked()), this, SLOT(saveModel()));
	connect (loadModelButton, SIGNAL (clicked()), this, SLOT(loadModel()));
	
	connect (assignMarkersButton, SIGNAL (clicked()), this, SLOT (assignMarkers()));
	connect (autoIKButton, SIGNAL (clicked()), this, SLOT (fitModel()));

	connect (fitAnimationButton, SIGNAL (clicked()), this, SLOT (fitAnimation()));
	connect (loadAnimationButton, SIGNAL (clicked()), this, SLOT (loadAnimation()));
	connect (saveAnimationButton, SIGNAL (clicked()), this, SLOT (saveAnimation()));
}

void print_usage(const char* execname) {
	cout << "Usage: " << execname << " <modelfile.lua> <mocapdata.c3d> <animation.csv>" << endl;
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
		else if (arg.substr(arg.size() - 4, 4) == ".csv")
			loadAnimationFile (arg.c_str());
		else {
			print_usage (argv[0]);
			return false;
		}
	}

	if (markerModel && animationData) {
		VectorNd state = animationData->getCurrentPose();
		markerModel->modelStateQ = state;
		markerModel->updateModelState();
		markerModel->updateSceneObjects();
	}

	if (markerModel && markerData) {
		modelFitter = new ModelFitter (markerModel, markerData);
		autoIKButton->setEnabled(true);
	}

	return true;
}

bool QtGLBaseApp::loadModelFile (const char* filename) {
	if (markerModel)
		delete markerModel;
	markerModel = new MarkerModel (scene);
	assert (markerModel);

	bool result = markerModel->loadFromFile (filename);
	updateModelStateEditor();
	return result;
}

bool QtGLBaseApp::loadMocapFile (const char* filename) {
	if (markerData)
		delete markerData;
	markerData = new MarkerData (scene);
	assert (markerData);

	if(!markerData->loadFromFile (filename)) 
		return false;

	slideMarkersCheckBox->setEnabled(true);
	slideMarkersCheckBox->setChecked(true);

	updateSliderBounds();

	return true;
}

bool QtGLBaseApp::loadAnimationFile (const char* filename) {
	if (animationData)
		delete animationData;
	
	animationData = new Animation();
	assert (animationData);

	if(!animationData->loadFromFile (filename))
		return false;

	slideAnimationCheckBox->setEnabled(true);
	slideAnimationCheckBox->setChecked(true);

	updateSliderBounds();

	return true;
}

void QtGLBaseApp::updateSliderBounds() {
	if (markerData || animationData)
		dockWidgetSlider->setVisible(true);
	else
		dockWidgetSlider->setVisible(false);

	dockWidgetSlider->setVisible(true);
	captureFrameSlider->setMinimum (markerData->getFirstFrame());
	captureFrameSlider->setMaximum (markerData->getLastFrame());
	connect (captureFrameSlider, SIGNAL (valueChanged(int)), this, SLOT (captureFrameSliderChanged (int)));
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

void QtGLBaseApp::loadModelState() {
	assert (markerModel);	
	markerModel->loadStateFromFile ("model_state.lua");
	updateModelStateEditor();
}

void QtGLBaseApp::saveModelState() {
	assert (markerModel);	
	markerModel->saveStateToFile ("model_state.lua");
}

void QtGLBaseApp::loadModel() {
	loadModelFile ("model.lua");
}

void QtGLBaseApp::saveModel() {
	markerModel->saveToFile ("model.lua");
}

void QtGLBaseApp::loadAnimation() {
	loadAnimationFile ("animation.csv");	
}

void QtGLBaseApp::saveAnimation() {
	assert (animationData);
	animationData->saveToFile ("animation.csv");
}

void QtGLBaseApp::collapseProperties() {
	// collapse all items
	QList<QtBrowserItem*>::ConstIterator prop_iter = propertiesBrowser->topLevelItems().constBegin();

	while (prop_iter != propertiesBrowser->topLevelItems().constEnd()) {
		propertiesBrowser->setExpanded (*prop_iter, false);
		prop_iter++;
	}
}

void QtGLBaseApp::objectSelected (int object_id) {
	activeModelFrame = 0;
	activeObject = object_id;

	if (markerModel && markerModel->isModelObject(object_id)) {
		activeModelFrame = markerModel->getFrameIdFromObjectId (object_id);
	}

	updateWidgetsFromObject (object_id);
	updateModelStateEditor();
}

void QtGLBaseApp::assignMarkers() {
	assert (markerData);
	assert (markerModel);
	
	int active_frame;
	std::vector<string> selected_marker_names;

	std::list<int>::iterator selected_iter;

	for (selected_iter = scene->selectedObjectIds.begin(); selected_iter != scene->selectedObjectIds.end(); selected_iter++) {
		if (markerData->isMarkerObject (*selected_iter)) {
			selected_marker_names.push_back (markerData->getMarkerName (*selected_iter));
		} else if (markerModel->isModelObject(*selected_iter)) {
			active_frame = markerModel->getFrameIdFromObjectId(*selected_iter);
		}
	}

	for (size_t i = 0; i < selected_marker_names.size(); i++) {
		Vector3f marker_position = markerData->getMarkerCurrentPosition (selected_marker_names[i].c_str());	
		Vector3f local_coords = markerModel->calcMarkerLocalCoords (active_frame, marker_position);
		markerModel->setFrameMarkerCoord (active_frame, selected_marker_names[i].c_str(), local_coords);
	}

	updateWidgetsFromObject (markerModel->getObjectIdFromFrameId (active_frame));
}

void QtGLBaseApp::fitModel() {
	if (!modelFitter)
		return;

	if (!markerModel)
		return;

	assert (modelFitter);
	bool success = modelFitter->run (markerModel->modelStateQ);
	if (success) {
		qDebug() << "fit successful!";
	} else {
		qDebug() << "fit failed!";
	}
	VectorNd q_fitted = modelFitter->getFittedState();
	markerModel->modelStateQ = q_fitted;
	markerModel->updateModelState();
	markerModel->updateSceneObjects();
}

void QtGLBaseApp::fitAnimation() {
	if (!modelFitter)
		return;

	if (!markerModel)
		return;

	if (!animationData)
		animationData = new Animation();

	animationData->keyFrames.clear();
	int frame_count = markerData->getLastFrame() - markerData->getFirstFrame();

	QProgressDialog progress ("Computing Animation...", "Cancel", 0, frame_count, this);
	progress.setWindowModality (Qt::WindowModal);
	progress.setMinimumDuration (0);

	bool success = true;

	for (int i = 0; i <= frame_count; i++) {
		progress.setValue(i);
		
		bool fit_result = modelFitter->computeModelAnimationFromMarkers (markerModel->modelStateQ, animationData, markerData->getFirstFrame() + i, markerData->getFirstFrame() + i);

		if (progress.wasCanceled()) {
			qDebug() << "canceled!";
			success = false;
			break;
		}

		if (!fit_result)
			success = false;
	}

	progress.setValue (frame_count);

	if (success) {
		qDebug() << "fit successful!";
	} else {
		qDebug() << "fit failed!";
	}

	slideAnimationCheckBox->setEnabled(true);
	slideAnimationCheckBox->setChecked(true);

	updateSliderBounds();
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

void QtGLBaseApp::updateModelStateEditor () {
	if (!markerModel) {
		modelStateEditor->setVisible(false);
		return;
	}
	dockModelStateEditor->setVisible(true);

	modelStateEditor->clear();

	VectorNd model_state = markerModel->getModelState();
	vector<string> state_names = markerModel->getModelStateNames();

	assert (model_state.size() == state_names.size());

	for (size_t i = 0; i < model_state.size(); i++) {
		QtProperty *dof_property = doubleManagerModelStateEditor->addProperty (state_names[i].c_str());
		doubleManagerModelStateEditor->setValue (dof_property, model_state[i]);
		doubleManagerModelStateEditor->setSingleStep (dof_property, 0.01);
		QtBrowserItem* item = modelStateEditor->addProperty (dof_property);
		if (markerModel->stateIndexIsFrameJointVariable(i, activeModelFrame)) {
			modelStateEditor->setBackgroundColor (item, QColor (180., 255., 180.));
		}
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

	// markers
	QtProperty *marker_group = groupManager->addProperty("Markers");
	vector<string> marker_names = markerModel->getFrameMarkerNames(frame_id);
	vector<Vector3f> marker_coords = markerModel->getFrameMarkerCoords(frame_id);
	for (size_t i = 0; i < marker_names.size(); i++) {
		QtProperty *marker_coordinates_property = vector3DPropertyManager->addProperty(marker_names[i].c_str());
		vector3DPropertyManager->setValue (marker_coordinates_property, QVector3D (marker_coords[i][0], marker_coords[i][1], marker_coords[i][2])); 
		registerProperty (marker_coordinates_property, "marker_coordinates");
		marker_group->addSubProperty (marker_coordinates_property);
	}
	item = propertiesBrowser->addProperty (marker_group);

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
	} else if (markerData && markerData->isMarkerObject(object_id)) {
		QtProperty *marker_name_property = stringManager->addProperty ("Name");
		stringManager->setValue (marker_name_property, markerData->getMarkerName (object_id).c_str());
		propertiesBrowser->insertProperty (marker_name_property, 0);
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
	updateWidgetsFromObject ((*scene->selectedObjectIds.end()));
}

void QtGLBaseApp::valueChanged (QtProperty *property, QVector3D value) {
	if (!propertyToName.contains(property))
		return;

	QString property_name = propertyToName[property];

	if (property_name.startsWith ("object_position")) {
		Vector3f position (value.x(), value.y(), value.z());
		scene->getObject<SceneObject>(activeObject)->transformation.translation = position;
	} else if (property_name.startsWith ("object_orientation")) {
		Vector3f yxz_rotation (value.x(), value.y(), value.z());
		Quaternion rotation = Quaternion::fromEulerYXZ (yxz_rotation);
		scene->getObject<SceneObject>(activeObject)->transformation.rotation = rotation;
	} else if (property_name.startsWith ("joint_location_local")) {
		Vector3f position (value.x(), value.y(), value.z());
		markerModel->setJointLocationLocal (activeModelFrame, position);
	} else if (property_name.startsWith ("joint_orientation_local")) {
		Vector3f yxz_rotation (value.x(), value.y(), value.z());
		Quaternion rotation = Quaternion::fromEulerYXZ (yxz_rotation);
		markerModel->setJointOrientationLocalEulerYXZ (activeModelFrame, rotation.toEulerYXZ());
	} else if (property_name.startsWith("marker_coordinates")) {
		Vector3f coord (value.x(), value.y(), value.z());
		markerModel->setFrameMarkerCoord (activeModelFrame, property->propertyName().toAscii(), coord);
	} else {
		qDebug() << "Warning! Unhandled value change of property " << property_name;
	}
}

void QtGLBaseApp::captureFrameSliderChanged (int value) {
	assert (markerData);

	if (slideMarkersCheckBox->isChecked() && slideAnimationCheckBox->isChecked()) {
		double first_frame = static_cast<double>(markerData->getFirstFrame());
		double last_frame = static_cast<double>(markerData->getLastFrame());
		double frame_rate = static_cast<double>(markerData->getFrameRate());

		double mocap_duration = (last_frame - first_frame) / frame_rate;
		if (fabs(animationData->getDuration() - mocap_duration) > 1.0e-3) {
			cerr << "Warning: duration mismatch: Animation = " << animationData->getDuration() << " Motion Capture data = " << mocap_duration << endl; 
		}
	}

	if (animationData) {
		double current_time = animationData->currentTime;
		int num_seconds = static_cast<int>(floor(current_time));
		int num_milliseconds = static_cast<int>(round((current_time - num_seconds) * 1000.f));

		stringstream time_string("");
		time_string << "Time: " << num_seconds << "." << setw(2) << setfill('0') << num_milliseconds;
		timeLabel->setText(time_string.str().c_str());
	}

	if (markerData) {
		stringstream frame_string("");
		frame_string << "Frame: " << markerData->currentFrame;
		frameLabel->setText(frame_string.str().c_str());
	}

	if (slideMarkersCheckBox->isChecked()) 
		markerData->setCurrentFrameNumber (value);

	if (autoIKButton->isChecked())
		fitModel();

	if (slideAnimationCheckBox->isChecked()) {
		double slider_min = static_cast<double> (captureFrameSlider->minimum());	
		double slider_max = static_cast<double> (captureFrameSlider->maximum());
		double value_d = static_cast<double>(value);
		double slider_percentage = (value_d - slider_min) / (slider_max - slider_min);

		animationData->setCurrentTime (animationData->getFirstFrameTime() + animationData->getDuration() * slider_percentage);

		VectorNd state = animationData->getCurrentPose();
		if (state.size() < markerModel->modelStateQ.size()) {
			cerr << "Error: animation has fewer values than model has states!" << endl;
			abort();
		}
		for (size_t i = 0; i < markerModel->modelStateQ.size(); i++) {
			markerModel->modelStateQ[i] = state[i];
		}

		markerModel->updateModelState();
		markerModel->updateSceneObjects();
	}

	updateModelStateEditor();
}
