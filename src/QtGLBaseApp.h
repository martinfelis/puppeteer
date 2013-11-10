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

struct Scene;
struct MarkerModel;
struct MarkerData;

class QtGLBaseApp : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    QtGLBaseApp(QWidget *parent = 0);
		virtual ~QtGLBaseApp();

		bool parseArgs(int argc, char* argv[]);
		bool loadModelFile (const char* filename);
		bool loadMocapFile (const char* filename);

protected:
		QTimer *drawTimer;
		Scene *scene;
		MarkerModel *markerModel;
		MarkerData *markerData;
		int activeModelFrame;
		int activeObject;

		QtVector3DPropertyManager *vector3DPropertyManager;
		QtVector3DPropertyManager *vector3DReadOnlyPropertyManager;
		QtVector3DPropertyManager *vector3DYXZPropertyManager;
		QtVector3DPropertyManager *vector3DYXZReadOnlyPropertyManager;
		QtDoublePropertyManager *doubleReadOnlyManager;
		QtStringPropertyManager *stringManager;
		QtColorPropertyManager *colorManager;
		QtDoublePropertyManager *doubleManagerModelStateEditor;

		QtVector3DEditorFactory *vector3DEditorFactory;
		QtVector3DEditorFactory *vector3DYXZEditorFactory;
		QtDoubleSpinBoxFactory *doubleSpinBoxFactory;
		QtDoubleSpinBoxFactory *doubleSpinBoxFactoryModelStateEditor;
		QtLineEditFactory *lineEditFactory;
		QtColorEditorFactory *colorEditFactory;
		QtGroupPropertyManager *groupManager;

		QMap<QtProperty *, QString> propertyToName;
		QMap<QString, QtProperty *> nameToProperty;
		QMap<QString, QtBrowserItem *> idToItem;
		QMap<QString, bool> idToExpanded;
		QMap<QtProperty *, unsigned int> propertyToStateIndex;

		void updateModelStateEditor();
		void updateExpandStateRecursive (const QList<QtBrowserItem *> &list, const QString &parent_property_id);
		void restoreExpandStateRecursive (const QList<QtBrowserItem *> &list, const QString &parent_property_id);
		void registerProperty (QtProperty *property, const QString &name) {
			propertyToName[property] = name;
			nameToProperty[name] = property;
		}

public slots:
		void quitApplication();

		void loadModelState();
		void saveModelState();
		void collapseProperties();
		void objectSelected(int object_id);
		void updateWidgetsFromObject (int object_id);
		void updatePropertiesForFrame (unsigned int frame_id);

		void modelStateValueChanged (QtProperty *property, double value);
		void valueChanged(QtProperty *property, QVector3D value);
		void captureFrameSliderChanged (int value);
};
 
#endif
