/*
 * QtGLAppBase - Simple Qt Application to get started with OpenGL stuff.
 *
 * Copyright (c) 2011-2012 Martin Felis <martin.felis@iwr.uni-heidelberg.de>
 *
 * Licensed under the MIT license. See LICENSE for more details.
 */

#include <QApplication>

#include "QtGLBaseApp.h"

#include <iostream>
#include <clocale>

using namespace std;

int main(int argc, char *argv[])
{
	setlocale (LC_NUMERIC, "C");
	QApplication app(argc, argv);
	QtGLBaseApp *main_window = new QtGLBaseApp;

	main_window->show();
	if (!main_window->parseArgs (argc, argv)) {
		cerr << "Invalid arguments!" << endl;
		return 0;
	}

	return app.exec();
}
