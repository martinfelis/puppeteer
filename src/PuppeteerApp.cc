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

#include <GL/glew.h>
#include <QtGui> 
#include <QProgressDialog>
#include <QFileDialog>
#include <QRegExp>
#include <QRegExpValidator>
#include <QProgressDialog>
#include <QMessageBox>

#include "GLWidget.h" 
#include "PuppeteerApp.h"
#include "QtVariantPropertyManager"

#include "Scene.h"
#include "Model.h"
#include "MarkerData.h"
#include "ModelFitter.h"
#include "Animation.h"
#include "Scripting.h"

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

const double TIME_SLIDER_RATE = 1000.;

PuppeteerApp::~PuppeteerApp() {
	if (scene) {
		delete scene;
		scene = NULL;
	}

	if (dataChart) {
		delete dataChart;
		dataChart = NULL;
	}

	scene = NULL;
}

PuppeteerApp::PuppeteerApp(QWidget *parent)
{
	setupUi(this); // this sets up GUI

	L = NULL;

	// create Scene
	scene = new Scene;	
	glWidget->setScene (scene);

	aboutDialog = new PuppeteerAboutDialog(this);

	markerModel = NULL;
	markerData = NULL;
	modelFitter = NULL;
	animationData = NULL;
	activeModelFrame = 0;
	activeObject = -1;

	drawTimer = new QTimer (this);
	drawTimer->setSingleShot(false);
	drawTimer->start(20);

	playbackTimer = new QTimer (this);
	previousPlaybackTime.start();
	
	updateTime.start();

	dataChart = new ChartContainer("Joint Trajectories", "t in [s]", "Angle in [RAD]", true);

	dockModelStateEditor->setVisible(true);
	dockDataChart->setVisible(false);
	
	dockWidgetSlider->setVisible(false);
	autoIKButton->setEnabled(false);

	slideAnimationCheckBox->setEnabled(false);
	slideMarkersCheckBox->setEnabled(false);

	drawMocapMarkersCheckBox->setEnabled(false);
	drawModelMarkersCheckBox->setEnabled(false);
	drawBodySegmentsCheckBox->setEnabled(false);
	drawJointsCheckBox->setEnabled(false);
	drawPointsCheckBox->setEnabled(false);

	connect (actionFrontView, SIGNAL (triggered()), glWidget, SLOT (set_front_view()));
	connect (actionSideView, SIGNAL (triggered()), glWidget, SLOT (set_side_view()));
	connect (actionTopView, SIGNAL (triggered()), glWidget, SLOT (set_top_view()));
	connect (actionToggleOrthographic, SIGNAL (toggled(bool)), glWidget, SLOT (toggle_draw_orthographic(bool)));

	// actionQuit() makes sure to set the settings before we quit
	connect (actionQuit, SIGNAL( triggered() ), this, SLOT( quitApplication() ));
	connect (actionAbout, SIGNAL( triggered() ), this, SLOT( execAboutDialog() ));

	// call the drawing function
	connect (drawTimer, SIGNAL(timeout()), this, SLOT (drawScene()));

	// object selection
	connect (glWidget, SIGNAL(object_selected(int)), this, SLOT (objectSelected(int)));
	connect (glWidget, SIGNAL(object_unselected(int)), this, SLOT (objectUnSelected(int)));

	// property browser: managers
	doubleReadOnlyManager = new QtDoublePropertyManager(propertiesBrowser);
	doubleManager = new QtDoublePropertyManager(propertiesBrowser);
	stringManager = new QtStringPropertyManager(propertiesBrowser);
	colorManager = new QtColorPropertyManager(propertiesBrowser);
	groupManager = new QtGroupPropertyManager(propertiesBrowser);
	vector3DPropertyManager = new QtVector3DPropertyManager (propertiesBrowser);
	vector3DReadOnlyPropertyManager = new QtVector3DPropertyManager (propertiesBrowser);	
	vector3DYXZPropertyManager = new QtVector3DPropertyManager (propertiesBrowser);	
	vector3DYXZReadOnlyPropertyManager = new QtVector3DPropertyManager (propertiesBrowser);
	vector3DYXZPropertyManager->setPropertyLabels ("Y", "X", "Z");

	vector3DPropertyManager->setDefaultDecimals (4);
	vector3DYXZPropertyManager->setDefaultDecimals (4);

	// property browser: editor factories
	doubleSpinBoxFactory = new QtDoubleSpinBoxFactory(propertiesBrowser);
	lineEditFactory = new QtLineEditFactory(propertiesBrowser);
	colorEditFactory = new QtColorEditorFactory(propertiesBrowser);
	vector3DEditorFactory = new QtVector3DEditorFactory(propertiesBrowser);
	vector3DYXZEditorFactory = new QtVector3DEditorFactory(propertiesBrowser);

	// model state editor
	colorManagerModelStateEditor = new QtColorPropertyManager(modelStateEditor);
	colorEditFactoryModelStateEditor = new QtColorEditorFactory(modelStateEditor);
	modelStateEditor->setFactoryForManager(colorManagerModelStateEditor, colorEditFactoryModelStateEditor);
	
	doubleManagerModelStateEditor = new QtDoublePropertyManager(modelStateEditor);
	doubleSpinBoxFactoryModelStateEditor = new QtDoubleSpinBoxFactory(modelStateEditor);
	modelStateEditor->setFactoryForManager (doubleManagerModelStateEditor, doubleSpinBoxFactoryModelStateEditor);

	enumManagerModelStateEditor = new QtEnumPropertyManager(modelStateEditor);
	visibility_types << "yes" << "no";
	enumFactoryModelStateEditor = new QtEnumEditorFactory(modelStateEditor);
	modelStateEditor->setFactoryForManager(enumManagerModelStateEditor, enumFactoryModelStateEditor);

	// property browser: manager <-> editor
	propertiesBrowser->setFactoryForManager (doubleManager, doubleSpinBoxFactory);
	propertiesBrowser->setFactoryForManager (stringManager, lineEditFactory);
	propertiesBrowser->setFactoryForManager (colorManager, colorEditFactory);
	propertiesBrowser->setFactoryForManager (vector3DPropertyManager, vector3DEditorFactory);
	propertiesBrowser->setFactoryForManager (vector3DPropertyManager->subDoublePropertyManager(), doubleSpinBoxFactory);
	propertiesBrowser->setFactoryForManager (vector3DYXZPropertyManager, vector3DEditorFactory);
	propertiesBrowser->setFactoryForManager (vector3DYXZPropertyManager->subDoublePropertyManager(), doubleSpinBoxFactory);

	// setup chart container
	
	dataChart->registerParent(vtkChartContainer);
	// VectorNd time = VectorNd::Zero(3);
	// VectorNd state = VectorNd::Zero(3);
	// state[1] = 0.5;
	// state[2] = 1.0;
	// time[1] = 0.5;
	// time[2] = 1.0;

	// dataChart->pushData("Title", time, state, 1.0, ChartColor(255,0,0,255));

	// signals
	connect (doubleManager, SIGNAL (valueChanged(QtProperty *, double)), this, SLOT (valueChanged (QtProperty *, double)));
	connect (doubleManagerModelStateEditor, SIGNAL (valueChanged(QtProperty *, double)), this, SLOT (modelStateValueChanged (QtProperty *, double)));
	connect (enumManagerModelStateEditor, SIGNAL (valueChanged(QtProperty *, int)), this, SLOT (modelStatePlotVisibleChanged (QtProperty *, int)));
	connect (colorManagerModelStateEditor, SIGNAL (valueChanged(QtProperty *, QColor)), this, SLOT (modelStatePlotColorChanged (QtProperty *, QColor)));

	connect (vector3DPropertyManager, SIGNAL (valueChanged(QtProperty *, QVector3D)), this, SLOT (valueChanged (QtProperty *, QVector3D)));
	connect (vector3DYXZPropertyManager, SIGNAL (valueChanged(QtProperty *, QVector3D)), this, SLOT (valueChanged (QtProperty *, QVector3D)));
	connect (colorManager, SIGNAL (valueChanged (QtProperty *, QColor)), this, SLOT (colorValueChanged (QtProperty *, QColor)));

	// Loading and saving of model and animation data
	connect (saveModelStateButton, SIGNAL (clicked()), this, SLOT (saveModelState()));
	connect (loadModelStateButton, SIGNAL (clicked()), this, SLOT (loadModelState()));
	connect (actionSaveModel, SIGNAL (triggered()), this, SLOT(saveModel()));
	connect (actionSaveModelAs, SIGNAL (triggered()), this, SLOT(saveModelDialog()));
	connect (actionLoadModel, SIGNAL (triggered(bool)), this, SLOT(loadModelDialog()));
	connect (actionLoadMotionCaptureData, SIGNAL (triggered(bool)), this, SLOT(loadMotionCaptureDataDialog()));
	connect (actionExportAnimationAsCSV, SIGNAL (triggered()), this, SLOT(exportAnimationDialog()));
	connect (this, SIGNAL(model_loaded()), this, SLOT( updateWidgetValidity()));
	connect (this, SIGNAL(motion_capture_data_loaded()), this, SLOT( updateWidgetValidity()));
	connect (this, SIGNAL(animation_fitting_complete()), this, SLOT( updateWidgetValidity()));
	
	connect (assignMarkersButton, SIGNAL (clicked()), this, SLOT (assignMarkers()));
	connect (autoIKButton, SIGNAL (clicked()), this, SLOT (fitModel()));

	connect (fitAnimationButton, SIGNAL (clicked()), this, SLOT (fitAnimation()));
	connect (loadAnimationButton, SIGNAL (clicked()), this, SLOT (loadAnimation()));
	connect (saveAnimationButton, SIGNAL (clicked()), this, SLOT (saveAnimation()));

	connect (drawMocapMarkersCheckBox, SIGNAL (stateChanged(int)), this, SLOT (displayMocapMarkers(int)));
	connect (drawModelMarkersCheckBox, SIGNAL (stateChanged(int)), this, SLOT (displayModelMarkers(int)));
	connect (drawBodySegmentsCheckBox, SIGNAL (stateChanged(int)), this, SLOT (displayBodySegments(int)));
	connect (drawJointsCheckBox, SIGNAL (stateChanged(int)), this, SLOT (displayJoints(int)));
	connect (drawPointsCheckBox, SIGNAL (stateChanged(int)), this, SLOT (displayPoints(int)));

	connect (toolButtonPlay, SIGNAL (clicked(bool)), this, SLOT (playButtonClicked(bool)));

	updateWidgetValidity();
}

void print_usage(const char* execname) {
	cout << "Usage: " << execname << " <modelfile.lua> <mocapdata.c3d> <animation.csv> [-s scriptfile.lua]" << endl;
}

bool PuppeteerApp::parseArgs(int argc, char* argv[]) {
	string scripting_file = "";

	try {
		// the basic definition of the command parser line cites a description for the 
		// program the delimiter to be applied on the command line as well as the
		// version number of the program
		TCLAP::CmdLine cmd("puppeteer - motion capture fitting for the masses",' ', "0.1");

		// now we have to populate the command line object by adding command line
		// options to it. These command line options are adjusted to a specific
		// data type by means of a template argument. The constructor features
		// the following arguments by order:
		// # (const char*) option
		// # (const char*) name of the option
		// # (const char*) description of the option
		// # (const char*) boolean to specify whether the argument is required to startup
		// # (const char*) a default value 
		// # (const char*) the data type of the value in const char*
		// In our case we will use an unlabeled command line options. The only
		// difference is now that the option parameter is missing. 
		//
		// NOTE: The order the unlabeled command line options are added to the 
		//  parser is the order by which they are then parsed on the command line

		// [mandatory] command line option to specify the experiment to be loaded
		// this always comes last
		TCLAP::UnlabeledMultiArg<string> files_Arg("files", "<modelfile.lua> <mocapdata.c3d> <animation.csv>", false,"", "string");

		// [optional] command line option to specify overwrite the output simulation
		TCLAP::SwitchArg rotateMoCap_Swi("r", "rotate", "rotate Motion capturing data", false);

		TCLAP::ValueArg<string> scripting_file_arg ("s", "script", "scripting file", false, "", "");

		// than we may add the command line option to the command line parser
		cmd.add( files_Arg );
		cmd.add( rotateMoCap_Swi );
		cmd.add( scripting_file_arg );

		// then we do parse the command line
		cmd.parse(argc, argv);

		scripting_file = scripting_file_arg.getValue();

		vector<string> files = files_Arg.getValue();
		for (vector<string>::iterator filePtr = files.begin(); filePtr != files.end(); filePtr++) {
			string arg (*filePtr);
			if (arg.substr(arg.size() - 4, 4) == ".lua")
				loadModelFile (arg.c_str());
			else if (arg.substr(arg.size() - 4, 4) == ".c3d")
				loadMocapFile (arg.c_str(), rotateMoCap_Swi.getValue());
			else if (arg.substr(arg.size() - 4, 4) == ".csv")
				loadAnimationFile (arg.c_str());
		}
	} catch (TCLAP::ArgException &e) {  // here we treat exception that may come 
    // due to corrupt command line options
    std::cerr << "Error: " << e.error() << " for arg " << e.argId() << std::endl;
    abort();
  }

	if (markerModel) {
		drawModelMarkersCheckBox->setEnabled(true);
		drawBodySegmentsCheckBox->setEnabled(true);
		drawJointsCheckBox->setEnabled(true);
		drawPointsCheckBox->setEnabled(true);

		displayModelMarkers (drawModelMarkersCheckBox->checkState());
		displayBodySegments (drawBodySegmentsCheckBox->checkState());
		displayJoints (drawJointsCheckBox->checkState());
	}

	if (markerModel && animationData) {
		VectorNd state = animationData->getCurrentPose();
		markerModel->modelStateQ = state;
		markerModel->updateModelState();
		markerModel->updateSceneObjects();
		updateModelStateEditor();
	}

	if (markerData) {
		drawMocapMarkersCheckBox->setEnabled(true);
		if (markerModel) {
			modelFitter = new SugiharaFitter (markerModel, markerData);
			//		modelFitter = new LevenbergMarquardtFitter (markerModel, markerData);
			autoIKButton->setEnabled(true);
		}
	}

	if (scripting_file != "") {
		scripting_init (this, scripting_file.c_str());
	} else {
		scripting_init (this, NULL);
	}

	if (L) {
		int script_args_start = argc;
		for (int i = 0; i < argc; i++) {
			if (i < argc -2 && (string("-s") == argv[i] || string("--script") == argv[i])) {
				script_args_start = i + 2;
				break;
			}
		}
		scripting_load (L, argc - script_args_start, &argv[script_args_start]);
	}

	updateWidgetValidity();

	return true;
}

bool PuppeteerApp::loadModelFile (const char* filename) {
	if (markerModel)
		delete markerModel;
	markerModel = new Model(scene);
	assert (markerModel);

	bool result = markerModel->loadFromFile (filename);
	markerModel->fileName = filename;
	buildModelStateEditor();
	updateModelStateEditor();

	emit model_loaded();

	return result;
}

bool PuppeteerApp::loadMocapFile (const char* filename, const bool rotateZ) {
	if (markerData)
		delete markerData;
	markerData = new MarkerData (scene);
	assert (markerData);

	for (int i=0;i<markerModel->modelMarkers.size();i++)
		markerData->markerNames.push_back(markerModel->modelMarkers[i]->markerName);

	if(!markerData->loadFromFile (filename)) 
		return false;

	// check whether we want to rotate the data
	if (markerData->markerExists ("LASI") && markerData->markerExists ("LPSI")) {

		float fraction_negative = 0.f;
		int frame_count = markerData->getLastFrame() - markerData->getFirstFrame();

		for (int i = markerData->getFirstFrame(); i < markerData->getLastFrame(); i++) {
			markerData->setCurrentFrameNumber (i);
			Vector3f lasi = markerData->getMarkerCurrentPosition("LASI");
			Vector3f lpsi = markerData->getMarkerCurrentPosition("LPSI");

			float projection = (lasi - lpsi).normalize().dot(Vector3f (1.f, 0.f, 0.f));

			if (projection < -0.)
				fraction_negative = fraction_negative + 1.f / static_cast<float>(frame_count);
		}

		// reset current frame
		markerData->setCurrentFrameNumber (markerData->getFirstFrame());

		if (fraction_negative > 0.5) {
			QMessageBox rotate_message_box;
			rotate_message_box.setText("Backwards orientation detected.");
			rotate_message_box.setInformativeText ("Re-align subject orientation along positive X axis?");
			rotate_message_box.setDetailedText ("The subject of the motion capture trial is facing along the negative X axis for the majority of the frames. This can result in numerical problems when fitting and it is highly recommended to re-align the data for the subject.");
			rotate_message_box.setStandardButtons (QMessageBox::Yes | QMessageBox::No);
			rotate_message_box.setIcon (QMessageBox::Question);

			rotate_message_box.setDefaultButton (QMessageBox::Save);

			int ret = rotate_message_box.exec();

			if (ret == QMessageBox::Yes) {
				markerData->rotateZ = true;
				markerData->updateMarkerSceneObjects();
			}
		}
	} else if (rotateZ) {
	  markerData->rotateZ = true;
	  markerData->updateMarkerSceneObjects();
	  std::cout << "-- INFO::Marker globally rotated about Z axis (as you requested)" << std::endl;
	}

	slideMarkersCheckBox->setEnabled(true);
	slideMarkersCheckBox->setChecked(true);

	updateSliderBounds();

	emit motion_capture_data_loaded();

	return true;
}

bool PuppeteerApp::loadAnimationFile (const char* filename) {
	if (animationData)
		delete animationData;
	
	animationData = new Animation();
	assert (animationData);

	if(!animationData->loadFromFile (filename))
		return false;

	updateSliderBounds();
	captureFrameSliderChanged (captureFrameSlider->minimum());

	updateGraph();

	return true;
}

void PuppeteerApp::updateSliderBounds() {
	if (markerData || animationData)
		dockWidgetSlider->setVisible(true);
	else
		dockWidgetSlider->setVisible(false);

	if (markerModel && animationData) {
		slideAnimationCheckBox->setEnabled(true);
		slideAnimationCheckBox->setChecked(true);
	} else {
		slideAnimationCheckBox->setEnabled(false);
		slideAnimationCheckBox->setChecked(false);
	}

	if (markerData) {
		dockWidgetSlider->setEnabled(true);
		dockWidgetSlider->setVisible(true);

		double duration = static_cast<double>(markerData->getLastFrame() - markerData->getFirstFrame()) / static_cast<double>(markerData->getFrameRate());

		captureFrameSlider->setMinimum (0);
		captureFrameSlider->setMaximum (static_cast<int>(floor(duration * TIME_SLIDER_RATE)));
		connect (captureFrameSlider, SIGNAL (valueChanged(int)), this, SLOT (captureFrameSliderChanged (int)));
	} else {
		dockWidgetSlider->setEnabled(true);
		captureFrameSlider->setMinimum (0);
		captureFrameSlider->setMaximum (static_cast<int>(floor(animationData->getDuration() * TIME_SLIDER_RATE)));
		connect (captureFrameSlider, SIGNAL (valueChanged(int)), this, SLOT (captureFrameSliderChanged (int)));
	}
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

void PuppeteerApp::execAboutDialog() {
	aboutDialog->exec();
}

void PuppeteerApp::quitApplication() {
	qApp->quit();
}

void PuppeteerApp::loadModelState() {
	assert (markerModel);	
	markerModel->loadStateFromFile ("model_state.lua");
	updateModelStateEditor();
}

void PuppeteerApp::saveModelState() {
	assert (markerModel);	
	markerModel->saveStateToFile ("model_state.lua");
}

void PuppeteerApp::loadModel() {
	loadModelFile ("model.lua");
}

void PuppeteerApp::saveModel() {
	if (markerModel->fileName != "") {
		markerModel->saveToFile (markerModel->fileName.c_str());
	}
}

void PuppeteerApp::loadAnimation() {
	loadAnimationFile ("animation.csv");	
}

void PuppeteerApp::saveAnimation() {
	assert (animationData);
	animationData->saveToFile ("animation.csv");
}

void PuppeteerApp::collapseProperties() {
	// collapse all items
	QList<QtBrowserItem*>::ConstIterator prop_iter = propertiesBrowser->topLevelItems().constBegin();

	while (prop_iter != propertiesBrowser->topLevelItems().constEnd()) {
		propertiesBrowser->setExpanded (*prop_iter, false);
		prop_iter++;
	}
}

void PuppeteerApp::selectionChanged() {
	activeModelFrame = 0;

	int active_frame = 0;
	int selected_markers_count = 0;
	std::list<int>::iterator selected_iter;

	for (selected_iter = scene->selectedObjectIds.begin(); selected_iter != scene->selectedObjectIds.end(); selected_iter++) {
		if (markerData && markerData->isMarkerObject (*selected_iter)) {
			selected_markers_count ++;
		} else if (markerModel && markerModel->isModelObject(*selected_iter)) {
			active_frame = markerModel->getFrameIdFromObjectId(*selected_iter);
		}
	}

	if (markerModel && active_frame != 0) {
		activeModelFrame = active_frame;
	}

	if (activeModelFrame != 0 && selected_markers_count > 0) {
		assignMarkersButton->setEnabled(true);
	} else {
		assignMarkersButton->setEnabled(false);
	}
}

void PuppeteerApp::objectUnSelected (int object_id) {
	selectionChanged();

	updatePropertiesEditor (object_id);
	updateModelStateEditor();
}

void PuppeteerApp::objectSelected (int object_id) {
	activeObject = object_id;
	selectionChanged();

	if (markerModel && markerModel->isModelObject(object_id)) {
		activeModelFrame = markerModel->getFrameIdFromObjectId (object_id);
	} else {
		activeModelFrame = 0;
	}

	updatePropertiesEditor (object_id);
	updateModelStateEditor();
}

void PuppeteerApp::assignMarkers() {
	assert (markerData);
	assert (markerModel);
	
	int active_frame = 0;
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

	updatePropertiesEditor (markerModel->getObjectIdFromFrameId (active_frame));
}

void PuppeteerApp::updateGraph() {
    if (animationData) {
        std::vector<ChartColor> colorVec;
        std::vector<bool> visibleVec;

        colorVec.resize(markerModel->getModelState().size());
        visibleVec.resize(markerModel->getModelState().size());

        for (QMap<QtProperty *, unsigned int>::iterator propPtr = propertyToStateIndex.begin(); propPtr != propertyToStateIndex.end(); ++propPtr) {
            if (enumManagerModelStateEditor == (propPtr.key()->propertyManager())) {
                visibleVec[propPtr.value()] = ((dynamic_cast<QtEnumPropertyManager *>(propPtr.key()->propertyManager()))->value(propPtr.key()) == 0);
            }
            if (colorManagerModelStateEditor == (propPtr.key()->propertyManager())) {
                QColor plotColor = (dynamic_cast<QtColorPropertyManager *>(propPtr.key()->propertyManager()))->value(propPtr.key());

                colorVec[propPtr.value()] = ChartColor(plotColor.red(), plotColor.green(), plotColor.blue(), plotColor.alpha());
            }
        }

        vector<string> state_names = markerModel->getModelStateNames();
        dataChart->reset();

        VectorNd timeLine = animationData->getTimeLine();
        for (size_t idx = 0; idx < visibleVec.size(); idx++) {
            if (visibleVec[idx]) {
                VectorNd stateLine = animationData->getStateLine(idx);

                dataChart->pushData(state_names[idx], timeLine, stateLine, 0.50, colorVec[idx]);
            }
        }
        dataChart->update();
    }
}

void PuppeteerApp::updateWidgetValidity() {
	if (animationData && animationData->keyFrames.size() > 0) {
		actionExportAnimationAsCSV->setEnabled(true);
	} else {
		actionExportAnimationAsCSV->setEnabled(false);
	}

	if (markerModel) {
		actionSaveModel->setEnabled(true);
		actionSaveModelAs->setEnabled(true);

		drawModelMarkersCheckBox->setEnabled(true);
		drawBodySegmentsCheckBox->setEnabled(true);
		drawJointsCheckBox->setEnabled(true);
		drawPointsCheckBox->setEnabled(true);

		displayModelMarkers (drawModelMarkersCheckBox->checkState());
		displayBodySegments (drawBodySegmentsCheckBox->checkState());
		displayJoints (drawJointsCheckBox->checkState());
	} else {
		actionSaveModel->setEnabled(false);
		actionSaveModelAs->setEnabled(false);
	}
	
	if (markerData) {
		drawMocapMarkersCheckBox->setEnabled(true);
		if (markerModel) {
			modelFitter = new SugiharaFitter (markerModel, markerData);
			//		modelFitter = new LevenbergMarquardtFitter (markerModel, markerData);
			autoIKButton->setEnabled(true);
		}
	}

	if (markerModel && animationData) {
		VectorNd state = animationData->getCurrentPose();
		markerModel->modelStateQ = state;
		markerModel->updateModelState();
		markerModel->updateSceneObjects();
		updateModelStateEditor();
	}


}

void PuppeteerApp::saveModelDialog() {
	QString file_name = QFileDialog::getSaveFileName(this, tr("Save Model"),
			"./",
			tr("LuaModel Files (*.lua)"));

	if (file_name == "")
		return;

	markerModel->fileName = file_name.toStdString();
	saveModel();
}

void PuppeteerApp::loadModelDialog() {
	QString file_name = QFileDialog::getOpenFileName(this,
    tr("Open Model"), "./", tr("LuaModel Files (*.lua)"));

	if (file_name == "")
		return;

	loadModelFile (file_name.toLocal8Bit());
}

void PuppeteerApp::loadMotionCaptureDataDialog() {
	QString file_name = QFileDialog::getOpenFileName(this,
    tr("Open Motion Capture Data"), "./", tr("C3D Motion Capture Data (*.c3d)"));

	if (file_name == "")
		return;

	loadMocapFile (file_name.toLocal8Bit());
}

void PuppeteerApp::exportAnimationDialog() {
	QString file_name = QFileDialog::getSaveFileName(this, tr("Export Animation As .CSV File..."),
			"./",
			tr("CSV files (*.csv)"));

	if (file_name == "")
		return;

	assert (animationData);
	animationData->saveToFile (file_name.toLocal8Bit());
}

void PuppeteerApp::fitModel() {
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

void PuppeteerApp::fitAnimation() {
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
	int i = 0;

	for (i = 0; i <= frame_count; i++) {
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

	progress.setValue (i);

	if (success) {
		qDebug() << "fit successful!";
	} else {
		qDebug() << "fit failed!";
	}

	slideAnimationCheckBox->setEnabled(true);
	slideAnimationCheckBox->setChecked(true);

	updateSliderBounds();
	updateGraph();

	emit animation_fitting_complete();
}

void PuppeteerApp::updateExpandStateRecursive (const QList<QtBrowserItem *> &list, const QString &parent_property_id) {
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

void PuppeteerApp::restoreExpandStateRecursive (const QList<QtBrowserItem *> &list, const QString &parent_property_id) {
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
void PuppeteerApp::buildModelStateEditor() {
	if (!markerModel) {
		modelStateEditor->setVisible(false);
		return;
	}

	modelStateEditor->clear();

	VectorNd model_state = markerModel->getModelState();
	vector<string> state_names = markerModel->getModelStateNames();

	assert (model_state.size() == state_names.size());

	for (size_t i = 0; i < model_state.size(); i++) {
		QtProperty *dof_property = doubleManagerModelStateEditor->addProperty (state_names[i].c_str());
		doubleManagerModelStateEditor->setValue (dof_property, model_state[i]);
		doubleManagerModelStateEditor->setSingleStep (dof_property, 0.01);
		doubleManagerModelStateEditor->setDecimals (dof_property, 4);
		QtBrowserItem* item = modelStateEditor->addProperty (dof_property);
		if (markerModel->stateIndexIsFrameJointVariable(i, activeModelFrame)) {
			modelStateEditor->setBackgroundColor (item, QColor (180., 255., 180.));
		}
		propertyToStateIndex[dof_property] = i;
	}
}

void PuppeteerApp::updateModelStateEditor () {
	if (!dockModelStateEditor->isVisible())
		return;

	modelStateEditor->clear();

	VectorNd model_state = markerModel->getModelState();
	vector<string> state_names = markerModel->getModelStateNames();

	assert (model_state.size() == state_names.size());

	for (size_t i = 0; i < model_state.size(); i++) {
		QtProperty *dof_property = doubleManagerModelStateEditor->addProperty (state_names[i].c_str());
		doubleManagerModelStateEditor->setValue (dof_property, model_state[i]);
		doubleManagerModelStateEditor->setSingleStep (dof_property, 0.01);
		doubleManagerModelStateEditor->setDecimals (dof_property, 4);
		QtBrowserItem* item = modelStateEditor->addProperty (dof_property);
		if (markerModel->stateIndexIsFrameJointVariable(i, activeModelFrame)) {
			modelStateEditor->setBackgroundColor (item, QColor (180., 255., 180.));
		}
		propertyToStateIndex[dof_property] = i;
	}
}

void PuppeteerApp::updatePropertiesForFrame (unsigned int frame_id) {
	if (!dockPropertiesEditor->isVisible())
		return;

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
	if (item->children().size() > 0) {
		for (QList<QtBrowserItem*>::const_iterator iter = item->children().constBegin(); iter != item->children().constEnd(); iter++) {
			propertiesBrowser->setExpanded ((*iter), false);
		}
	}

	// visuals
	QtProperty *visuals_group = groupManager->addProperty("Visuals");
	for (int visual_id = 1; visual_id <= markerModel->getVisualsCount(frame_id); visual_id++) {
		ostringstream name_stream ("");
		name_stream << visual_id;
		string visual_name = name_stream.str();

		QtProperty *visual_property = groupManager->addProperty (visual_name.c_str());

		// center
		QtProperty *center_property = vector3DPropertyManager->addProperty("center");
		Vector3f center = markerModel->getVisualCenter (frame_id, visual_id);
		vector3DPropertyManager->setValue (center_property, QVector3D (center[0], center[1], center[2]));
		registerProperty (center_property, (std::string ("visuals_") + visual_name + "_center").c_str());
		visual_property->addSubProperty (center_property);

		// dimensions or scale
		if (markerModel->visualUsesDimensions (frame_id, visual_id)) {
			QtProperty *dimensions_property = vector3DPropertyManager->addProperty("dimensions");
			Vector3f dimensions = markerModel->getVisualDimensions (frame_id, visual_id);
			vector3DPropertyManager->setValue (dimensions_property, QVector3D (dimensions[0], dimensions[1], dimensions[2]));
			registerProperty (dimensions_property, (std::string ("visuals_") + visual_name + "_dimensions").c_str());
			visual_property->addSubProperty (dimensions_property);
		} else {
			QtProperty *scale_property = vector3DPropertyManager->addProperty("scale");
			Vector3f scale = markerModel->getVisualScale (frame_id, visual_id);
			vector3DPropertyManager->setValue (scale_property, QVector3D (scale[0], scale[1], scale[2]));
			registerProperty (scale_property, (std::string ("visuals_") + visual_name + "_scale").c_str());
			visual_property->addSubProperty (scale_property);
		}

		// color
		QtProperty *color_property = colorManager->addProperty("color");
		Vector3f color = markerModel->getVisualColor (frame_id, visual_id);
		colorManager->setValue (color_property, QColor (color[0] * 255, color[1] * 255, color[2] * 255));
		registerProperty (color_property, (std::string ("visuals_") + visual_name + "_color").c_str());
		visual_property->addSubProperty (color_property);

		registerProperty (visual_property, visual_name.c_str());
		visuals_group->addSubProperty (visual_property);
	}
	item = propertiesBrowser->addProperty (visuals_group);
	if (item->children().size() > 0) {
		for (QList<QtBrowserItem*>::const_iterator iter = item->children().constBegin(); iter != item->children().constEnd(); iter++) {
			propertiesBrowser->setExpanded ((*iter), false);
		}
	}

	// Inertial Properties
	QtProperty *body_group = groupManager->addProperty("Body");
	
	// mass
	QtProperty *mass_property = doubleManager->addProperty("mass");
	double mass = markerModel->getBodyMass (frame_id);
	doubleManager->setValue (mass_property, mass);
	registerProperty (mass_property, ("body_mass"));
	body_group->addSubProperty (mass_property);

	// com
	QtProperty *com_property = vector3DPropertyManager->addProperty("com");
	Vector3f com = markerModel->getBodyCOM (frame_id);
	vector3DPropertyManager->setValue (com_property, QVector3D (com[0], com[1], com[2]));
	registerProperty (com_property, "body_com");
	body_group->addSubProperty (com_property);

	// inertia
	QtProperty *inertia_group = groupManager->addProperty ("Inertia");

	Matrix33f inertia = markerModel->getBodyInertia (frame_id);

	// inertia row1
	QtProperty *inertia_row1_property = vector3DPropertyManager->addProperty("1");
	Vector3f inertia_row1 = inertia.block(0,0,3,1);
	vector3DPropertyManager->setValue (inertia_row1_property, QVector3D (inertia_row1[0], inertia_row1[1], inertia_row1[2]));
	registerProperty (inertia_row1_property, "body_inertia_row1");
	inertia_group->addSubProperty (inertia_row1_property);

	// inertia row2
	QtProperty *inertia_row2_property = vector3DPropertyManager->addProperty("2");
	Vector3f inertia_row2 = inertia.block(0,1,3,1);
	vector3DPropertyManager->setValue (inertia_row2_property, QVector3D (inertia_row2[0], inertia_row2[1], inertia_row2[2]));
	registerProperty (inertia_row2_property, "body_inertia_row2");
	inertia_group->addSubProperty (inertia_row2_property);

	// inertia row3
	QtProperty *inertia_row3_property = vector3DPropertyManager->addProperty("3");
	Vector3f inertia_row3 = inertia.block(0,2,3,1);
	vector3DPropertyManager->setValue (inertia_row3_property, QVector3D (inertia_row3[0], inertia_row3[1], inertia_row3[2]));
	registerProperty (inertia_row3_property, "body_inertia_row3");
	inertia_group->addSubProperty (inertia_row3_property);

	body_group->addSubProperty (inertia_group);

	item = propertiesBrowser->addProperty (body_group);
	if (item->children().size() > 0) {
		for (QList<QtBrowserItem*>::const_iterator iter = item->children().constBegin(); iter != item->children().constEnd(); iter++) {

//			if (iter)
				for (QList<QtBrowserItem*>::const_iterator iter2 = (*iter)->children().constBegin(); iter2 != (*iter)->children().constEnd(); iter2++) {
					propertiesBrowser->setExpanded ((*iter2), false);
				}
			propertiesBrowser->setExpanded ((*iter), false);
		}
	}

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

	restoreExpandStateRecursive(propertiesBrowser->topLevelItems(), "");
}

void PuppeteerApp::drawScene() {
	if (L)
		scripting_update (L, 1.0e-3f * static_cast<float>(updateTime.restart()) );
	glWidget->updateGL();

	if (L)
		scripting_draw (L);
}

void PuppeteerApp::playButtonClicked (bool checked) {
	if (!toolButtonPlay->isChecked()) {
		disconnect (playbackTimer, SIGNAL(timeout()), this, SLOT (advanceFrame()));
		playbackTimer->stop();
	} else {
		if (captureFrameSlider->value() == captureFrameSlider->maximum())
			captureFrameSlider->setValue (captureFrameSlider->minimum());

		playbackTimer->setSingleShot (false);
		playbackTimer->start(20);
		previousPlaybackTime.restart();

		connect (playbackTimer, SIGNAL(timeout()), this, SLOT (advanceFrame()));
	}
}

void PuppeteerApp::advanceFrame() {
	int current_pos = captureFrameSlider->value();
	int advancement = static_cast<int>(round(static_cast<double>(previousPlaybackTime.restart()) * playbackSpeedSpinBox->value()));

	if (current_pos + advancement > captureFrameSlider->maximum()) {
		if (repeatCheckBox->checkState() == Qt::Checked) {
			captureFrameSlider->setValue(captureFrameSlider->value() - captureFrameSlider->maximum() + advancement);
		} else {
			captureFrameSlider->setValue (captureFrameSlider->maximum());
			disconnect (playbackTimer, SIGNAL(timeout()), this, SLOT (advanceFrame()));
			playbackTimer->stop();
			toolButtonPlay->setChecked(false);
		}
	} else {
		captureFrameSlider->setValue(captureFrameSlider->value() + advancement);
	}
}

void PuppeteerApp::updatePropertiesEditor (int object_id) {
	if (object_id < 0) {
		propertiesBrowser->clear();
		return;
	}

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
		unsigned int frame_id = static_cast<unsigned int>(markerModel->getFrameIdFromObjectId (object_id));

		if (markerModel->isModelMarkerObject(object_id)) {
			ModelMarkerObject *marker_object = dynamic_cast<ModelMarkerObject*> (scene->getObject<SceneObject>(object_id));

			QtProperty *marker_name_property = stringManager->addProperty ("Name");
			stringManager->setValue (marker_name_property, marker_object->markerName.c_str());
			propertiesBrowser->insertProperty (marker_name_property, 0);
			QtProperty *marker_parent_property = stringManager->addProperty ("Parent");
			stringManager->setValue (marker_parent_property, markerModel->getFrameName (marker_object->frameId).c_str());
			propertiesBrowser->insertProperty (marker_parent_property, marker_name_property);
		} else if (markerModel->isContactPointObject (object_id)) {
			registerProperty (position_property, "contact_point_position_global");
			ContactPointObject *contact_point_object = dynamic_cast<ContactPointObject*> (scene->getObject<SceneObject>(object_id));

			QtProperty *contact_point_name_property = stringManager->addProperty ("Name");
			stringManager->setValue (contact_point_name_property, contact_point_object->name.c_str());
			propertiesBrowser->insertProperty (contact_point_name_property, 0);
			QtProperty *contact_point_parent_property = stringManager->addProperty ("Parent");
			stringManager->setValue (contact_point_parent_property, markerModel->getFrameName (contact_point_object->frameId).c_str());
			propertiesBrowser->insertProperty (contact_point_parent_property, contact_point_name_property);

			QtProperty *contact_point_local_property = vector3DPropertyManager->addProperty("Local Position");
			Vector3f contact_point_local = markerModel->getContactPointLocal (contact_point_object->pointIndex);
			vector3DPropertyManager->setValue (contact_point_local_property, QVector3D (contact_point_local[0], contact_point_local[1], contact_point_local[2]));
			vector3DPropertyManager->setSingleStep (contact_point_local_property, 0.01);
			registerProperty (contact_point_local_property, "contact_point_position_local");
			item = propertiesBrowser->addProperty (contact_point_local_property);
			propertiesBrowser->setExpanded (item, false);
		} else {
			updatePropertiesForFrame (frame_id);
		}
	} else if (markerData && markerData->isMarkerObject(object_id)) {
		QtProperty *marker_name_property = stringManager->addProperty ("Name");
		stringManager->setValue (marker_name_property, markerData->getMarkerName (object_id).c_str());
		propertiesBrowser->insertProperty (marker_name_property, 0);
	}

	restoreExpandStateRecursive(propertiesBrowser->topLevelItems(), "");
}

void PuppeteerApp::modelStateValueChanged (QtProperty *property, double value) {
	assert (markerModel);

	if (!propertyToStateIndex.contains(property)) {
		return;
	}

	unsigned int state_index = propertyToStateIndex[property];

	markerModel->setModelStateValue (state_index, value);	
	updatePropertiesEditor (activeObject);
}

void PuppeteerApp::modelStatePlotVisibleChanged (QtProperty *property, int state) {
	assert (markerModel);

	if (!propertyToStateIndex.contains(property) || !dockDataChart->isVisible()) {
		return;
	}

	updateGraph();
}

void PuppeteerApp::modelStatePlotColorChanged(QtProperty *property, QColor color) {
    assert (markerModel);

    if (!propertyToStateIndex.contains(property) || !dockDataChart->isVisible()) {
        return;
    }
    updateGraph();
}

void PuppeteerApp::valueChanged (QtProperty *property, double value) {
	if (!propertyToName.contains(property))
		return;

	QString property_name = propertyToName[property];

	if (property_name.startsWith ("body_mass")) {
		markerModel->setBodyMass (activeModelFrame, value);
	} else {
		qDebug() << "Warning! Unhandled value change of property " << property_name;
	}
}
	
void PuppeteerApp::valueChanged (QtProperty *property, QVector3D value) {
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
		markerModel->setFrameMarkerCoord (activeModelFrame, property->propertyName().toLatin1(), coord);
	}	else if (property_name.startsWith("visuals_")) {
		QRegExp rx("visuals_(\\d+)_(\\w+)");
		if (!rx.exactMatch (property_name)) {
			qDebug() << "Warning! Unhandled value change of property " << property_name;
			return;
		}

		int visual_id = rx.cap(1).toInt();
		QString visual_property = rx.cap(2);

		if (visual_property == "dimensions") {
			Vector3f dimensions (value.x(), value.y(), value.z());
			markerModel->setVisualDimensions (activeModelFrame, visual_id, dimensions);
		} else if (visual_property == "scale") {
			Vector3f scale (value.x(), value.y(), value.z());
			markerModel->setVisualScale (activeModelFrame, visual_id, scale);
		} else if (visual_property == "center") {
			Vector3f center (value.x(), value.y(), value.z());
			markerModel->setVisualCenter (activeModelFrame, visual_id, center);
		} else {
			qDebug() << "Warning! Unhandled value change of property " << property_name;
			return;
		}
	} else if (property_name.startsWith("body_com")) {
		markerModel->setBodyCOM (activeModelFrame, Vector3f (value.x(), value.y(), value.z()));
	} else if (property_name.startsWith("body_inertia_row")) {
		Matrix33f inertia = markerModel->getBodyInertia (activeModelFrame);
		if (property_name == "body_inertia_row1") {
			inertia.block<1,3>(0,0) = Vector3f (value.x(), value.y(), value.z()).transpose();
		} else if (property_name == "body_inertia_row2") {
			inertia.block<1,3>(1,0) = Vector3f (value.x(), value.y(), value.z()).transpose();
		} else if (property_name == "body_inertia_row3") {
			inertia.block<1,3>(2,0) = Vector3f (value.x(), value.y(), value.z()).transpose();
		} else {
			qDebug() << "Warning! Unhandled value change of property " << property_name;
		}
		markerModel->setBodyInertia (activeModelFrame, inertia);
	} else if (property_name.startsWith ("contact_point_position_global")) {
		ContactPointObject* contact_point_object = dynamic_cast<ContactPointObject*>(scene->getObject<SceneObject>(activeObject));

		if (!contact_point_object) {
			cerr << "Cannot update contact point object: object id " << activeObject << " is not a contact point!" << endl;
			abort();
		}
		
		Vector3f coords_global (value.x(), value.y(), value.z());
		markerModel->setContactPointGlobal (contact_point_object->pointIndex, coords_global);
	} else if (property_name.startsWith ("contact_point_position_local")) {
		ContactPointObject* contact_point_object = dynamic_cast<ContactPointObject*>(scene->getObject<SceneObject>(activeObject));

		if (!contact_point_object) {
			cerr << "Cannot update contact point object: object id " << activeObject << " is not a contact point!" << endl;
			abort();
		}
		
		Vector3f coords_local (value.x(), value.y(), value.z());
		markerModel->setContactPointLocal (contact_point_object->pointIndex, coords_local);
	} else {
		qDebug() << "Warning! Unhandled value change of property " << property_name;
	}
}

void PuppeteerApp::colorValueChanged (QtProperty *property, QColor value) {
	if (!propertyToName.contains(property))
		return;

	QString property_name = propertyToName[property];

	if (property_name.startsWith("visuals_")) {
		QRegExp rx("visuals_(\\d+)_(\\w+)");
		if (!rx.exactMatch (property_name)) {
			qDebug() << "Warning! Unhandled value change of property " << property_name;
			return;
		}

		int visual_id = rx.cap(1).toInt();
		QString visual_property = rx.cap(2);

		if (visual_property == "color") {
			Vector3f color (value.red() * 1.f/255.f, value.green() * 1.f/255.f , value.blue() * 1.f/255.f);
			markerModel->setVisualColor (activeModelFrame, visual_id, color);
		} else {
			qDebug() << "Warning! Unhandled value change of property " << property_name;
			return;
		}
	} else {
		qDebug() << "Warning! Unhandled value change of property " << property_name;
	}
}

void PuppeteerApp::captureFrameSliderChanged (int value) {
	assert (markerData || animationData);

	double current_time = static_cast<double>(value) / TIME_SLIDER_RATE;
	int current_frame = 0;

	dataChart->setTimePtr(current_time);
	
	if (markerData) {
		current_frame = markerData->getFirstFrame() + static_cast<int>(round(current_time * static_cast<double>(markerData->getFrameRate())));
	}

	if (activeObject >= 0)
		updatePropertiesEditor(activeObject);

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
		int num_fraction_digits = 2;
		int num_seconds = static_cast<int>(floor(current_time));
		int num_milliseconds = static_cast<int>(round((current_time - num_seconds) * pow(10, num_fraction_digits)));

		stringstream time_string("");
		time_string << "Time: " << num_seconds << "." << std::setfill('0') << std::setw(num_fraction_digits) << num_milliseconds;
		timeLabel->setText(time_string.str().c_str());
	}

	if (markerData) {
		stringstream frame_string("");
		frame_string << "Frame: " << markerData->currentFrame;
		frameLabel->setText(frame_string.str().c_str());
	}

	if (slideMarkersCheckBox->isChecked()) { 
		markerData->setCurrentFrameNumber (current_frame);
	}

	if (autoIKButton->isChecked())
		fitModel();

	if (slideAnimationCheckBox->isChecked()) {
		animationData->setCurrentTime(current_time);

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

void PuppeteerApp::displayMocapMarkers (int display_state) {
	bool no_draw = false;
	if (display_state != Qt::Checked)
		no_draw = true;

	for (int i = 0; i < markerData->markers.size(); i++) {
		markerData->markers[i]->noDraw = no_draw;
	}
}

void PuppeteerApp::displayModelMarkers (int display_state) {
	bool no_draw = false;
	if (display_state != Qt::Checked)
		no_draw = true;

	for (int i = 0; i < markerModel->modelMarkers.size(); i++) {
		markerModel->modelMarkers[i]->noDraw = no_draw;
	}
}

void PuppeteerApp::displayBodySegments (int display_state) {
	bool no_draw = false;
	if (display_state != Qt::Checked)
		no_draw = true;

	for (int i = 0; i < markerModel->visuals.size(); i++) {
		markerModel->visuals[i]->noDraw = no_draw;
	}
}

void PuppeteerApp::displayJoints (int display_state) {
	bool no_draw = false;
	if (display_state != Qt::Checked)
		no_draw = true;

	for (int i = 0; i < markerModel->joints.size(); i++) {
		markerModel->joints[i]->noDraw = no_draw;
	}
}

void PuppeteerApp::displayPoints (int display_state) {
	bool no_draw = false;
	if (display_state != Qt::Checked)
		no_draw = true;

	for (int i = 0; i < markerModel->contactPoints.size(); i++) {
		markerModel->contactPoints[i]->noDraw = no_draw;
	}
}

bool PuppeteerApp::saveScreenShot (const char* filename, int width, int height, bool alpha_channel) {
	QImage image = glWidget->renderContentOffscreen (width, height, alpha_channel);
	image.save (filename, 0, -1);
}

double PuppeteerApp::getCurrentTime () {
	return static_cast<double>(captureFrameSlider->value()) / TIME_SLIDER_RATE;
}

void PuppeteerApp::setCurrentTime (double time_in_seconds) {
	int value = static_cast<int>(time_in_seconds * TIME_SLIDER_RATE);
	captureFrameSlider->setValue (value);
}
