/*
 * QtGLAppBase - Simple Qt Application to get started with OpenGL stuff.
 *
 * Copyright (c) 2011-2012 Martin Felis <martin.felis@iwr.uni-heidelberg.de>
 *
 * Licensed under the MIT license. See LICENSE for more details.
 */

#include <QApplication>

#include "PuppeteerApp.h"

#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	PuppeteerApp *main_window = new PuppeteerApp;

	main_window->show();
	if (!main_window->parseArgs (argc, argv)) {
		cerr << "Invalid arguments!" << endl;
		return 0;
	}

	return app.exec();
}
