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

		QtDoublePropertyManager *doubleManager;
		QtStringPropertyManager *stringManager;
		QtColorPropertyManager *colorManager;

		QtDoubleSpinBoxFactory *doubleSpinBoxFactory;
		QtLineEditFactory *lineEditFactory;
		QtColorEditorFactory *colorEditFactory;
		QtGroupPropertyManager *groupManager;

		QMap<QtProperty *, QString> propertyToName;
		QMap<QString, QtProperty *> nameToProperty;
		QMap<QString, QtBrowserItem *> idToItem;
		QMap<QString, bool> idToExpanded;

		void updateExpandStateRecursive (const QList<QtBrowserItem *> &list, const QString &parent_property_id);
		void restoreExpandStateRecursive (const QList<QtBrowserItem *> &list, const QString &parent_property_id);
		void registerProperty (QtProperty *property, const QString &name) {
			propertyToName[property] = name;
			nameToProperty[name] = property;
		}

public slots:
		void cameraChanged ();
		void updateCamera ();
		void quitApplication();

		void collapseProperties();
		void updateWidgetsFromObject (int object_id);

		void valueChanged(QtProperty *property, double value);
};
 
#endif
