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
#include "ui_MainWindow.h"

struct Scene;
struct MarkerModel;

class QtGLBaseApp : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    QtGLBaseApp(QWidget *parent = 0);
		virtual ~QtGLBaseApp();

		bool parseArgs(int argc, char* argv[]);
		bool loadModelFile (const char* filename);

protected:
		QTimer *drawTimer;
		Scene *scene;
		MarkerModel *markerModel;
		QtVariantPropertyManager *variantManager;
		QtVariantEditorFactory *variantEditorFactory;
		QMap<QtProperty *, QSTring> propertyToId;

public slots:
		void cameraChanged ();
		void updateCamera ();
		void quitApplication();

		void propertyChanged(QtProperty *property, const QVariant &variant);

		void updateWidgetsFromObject (int object_id);
		void updateObjectFromWidget ();
};
 
#endif
