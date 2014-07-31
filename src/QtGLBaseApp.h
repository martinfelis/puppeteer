/*
 * QtGLAppBase - Simple Qt Application to get started with OpenGL stuff.
 *
 * Copyright (c) 2011-2012 Martin Felis <martin.felis@iwr.uni-heidelberg.de>
 *
 * Licensed under the MIT license. See LICENSE for more details.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>
#include <QTimeLine>
#include <QVector3D>
#include "QtVariantPropertyManager"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "ui_MainWindow.h"

#include "vtkChart/chartXY.h"

#include <boost/shared_ptr.hpp>
// commandline interpreter for convenient configuration of the modeltest
#include <tclap/CmdLine.h>

struct Scene;
struct MarkerModel;
struct MarkerData;
struct ModelFitter;
struct Animation;

class QtGLBaseApp : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    QtGLBaseApp(QWidget *parent = 0);
		virtual ~QtGLBaseApp();

		bool parseArgs(int argc, char* argv[]);
		bool loadModelFile (const char* filename);
		bool loadMocapFile (const char* filename, const bool rotateZ = false);
		bool loadAnimationFile (const char* filename);

protected:
		QTimer *drawTimer;
		QTimer *playbackTimer;
		QTime *previousPlaybackTime;
		Scene *scene;
		MarkerModel *markerModel;
		MarkerData *markerData;
		ModelFitter *modelFitter;
		Animation *animationData;
		int activeModelFrame;
		int activeObject;

		boost::shared_ptr<ChartContainer> dataChart;

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
		void quitApplication();
		void updateGraph();

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
};
 
#endif
