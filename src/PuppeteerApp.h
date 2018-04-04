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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTime>
#include <QTimer>
#include <QTimeLine>
#include <QVector3D>
#include "QtVariantPropertyManager"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "ui_PuppeteerMainWindow.h"
#include "PuppeteerAboutDialog.h"

extern "C" {
#include <lua5.1/lua.h>
#include <lua5.1/lualib.h>
#include <lua5.1/lauxlib.h>
}

#include "vtkChart/chartXY.h"

// commandline interpreter for convenient configuration of the modeltest
#include <tclap/CmdLine.h>

struct Scene;
struct Model;
struct MarkerData;
struct ModelFitter;
struct Animation;

class PuppeteerApp : public QMainWindow, public Ui::PuppeteerMainWindow
{
    Q_OBJECT

public:
    PuppeteerApp(QWidget *parent = 0);
		virtual ~PuppeteerApp();

		lua_State *L;
		Scene *scene;
		Model *markerModel;
		MarkerData *markerData;
		ModelFitter *modelFitter;
		Animation *animationData;

		PuppeteerAboutDialog *aboutDialog;

		bool parseArgs(int argc, char* argv[]);
		bool loadModelFile (const char* filename);
		bool loadMocapFile (const char* filename, const bool rotateZ = false);
		bool loadAnimationFile (const char* filename);
		bool saveScreenShot (const char* filename, int width, int height, bool alpha_channel);
		double getCurrentTime ();
		void setCurrentTime (double time_in_seconds);

protected:
		QTimer *drawTimer;
		QTimer *playbackTimer;
		QTime updateTime;
		QTime previousPlaybackTime;
		int activeModelFrame;
		int activeObject;

		ChartContainer* dataChart;

		QtVector3DPropertyManager *vector3DPropertyManager;
		QtVector3DPropertyManager *vector3DReadOnlyPropertyManager;
		QtVector3DPropertyManager *vector3DYXZPropertyManager;
		QtVector3DPropertyManager *vector3DYXZReadOnlyPropertyManager;
		QtDoublePropertyManager *doubleReadOnlyManager;
		QtDoublePropertyManager *doubleManager;
		QtStringPropertyManager *stringManager;
		QtColorPropertyManager *colorManager;
		QtDoublePropertyManager *doubleManagerModelStateEditor;
		QtColorPropertyManager *colorManagerModelStateEditor;
		QtEnumPropertyManager *enumManagerModelStateEditor;
		QStringList visibility_types;

		QtVector3DEditorFactory *vector3DEditorFactory;
		QtVector3DEditorFactory *vector3DYXZEditorFactory;
		QtDoubleSpinBoxFactory *doubleSpinBoxFactory;
		QtDoubleSpinBoxFactory *doubleSpinBoxFactoryModelStateEditor;
		QtEnumEditorFactory *enumFactoryModelStateEditor;
		QtColorEditorFactory *colorEditFactoryModelStateEditor;
		QtLineEditFactory *lineEditFactory;
		QtColorEditorFactory *colorEditFactory;
		QtGroupPropertyManager *groupManager;

		QMap<QtProperty *, QString> propertyToName;
		QMap<QString, QtProperty *> nameToProperty;
		QMap<QString, QtBrowserItem *> idToItem;
		QMap<QString, bool> idToExpanded;
		QMap<QtProperty *, unsigned int> propertyToStateIndex;

		void updateModelStateEditor();
		void buildModelStateEditor();
		void updateExpandStateRecursive (const QList<QtBrowserItem *> &list, const QString &parent_property_id);
		void restoreExpandStateRecursive (const QList<QtBrowserItem *> &list, const QString &parent_property_id);
		void registerProperty (QtProperty *property, const QString &name) {
			propertyToName[property] = name;
			nameToProperty[name] = property;
		}
		void updateSliderBounds ();

public slots:
		void execAboutDialog();

		void quitApplication();
		void updateGraph();
		void updateWidgetValidity();

		void saveModelDialog();
		void loadModelDialog();
		void loadMotionCaptureDataDialog();
		void exportAnimationDialog();

		void loadModelState();
		void saveModelState();
		void loadModel();
		void saveModel();
		void loadAnimation();
		void saveAnimation();
		void collapseProperties();
		void objectSelected(int object_id);
		void objectUnSelected(int object_id);
		void selectionChanged();
		void assignMarkers();
		void fitModel();
		void fitAnimation();

		void updatePropertiesEditor (int object_id);
		void updatePropertiesForFrame (unsigned int frame_id);

		void drawScene();
		void playButtonClicked (bool checked);
		void advanceFrame ();

		void modelStateValueChanged (QtProperty *property, double value);
		void modelStatePlotVisibleChanged (QtProperty *property, int state);
		void modelStatePlotColorChanged (QtProperty *property, QColor color);
		void valueChanged(QtProperty *property, double value);
		void valueChanged(QtProperty *property, QVector3D value);
		void colorValueChanged(QtProperty *property, QColor value);
		void captureFrameSliderChanged (int value);

		void displayMocapMarkers (int display_state);
		void displayModelMarkers (int display_state);
		void displayBodySegments (int display_state);
		void displayJoints (int display_state);
		void displayPoints (int display_state);

	signals:
		void animation_fitting_complete();
		void model_loaded();
		void motion_capture_data_loaded ();

};
 
#endif
