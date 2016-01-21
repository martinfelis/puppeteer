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

#include <iostream>
#include <sstream>

#include "timer.h"

#include "Model.h"
#include "MarkerData.h"
#include "Animation.h"
#include "ModelFitter.h"

/* workaround when using ubuntu versions (e.g. 14.04) that are affected by
 * https://bugs.launchpad.net/ubuntu/+source/nvidia-graphics-drivers-319/+bug/1248642
 * see also CMakeLists.txt where fit_motion is explicitly linked against
 * pthread.
 */
#include <pthread.h>
void fix_nvidia_linking_new_ubuntu () {
	int i;
	i = pthread_getconcurrency();
}

using namespace std;

Model *model = NULL;
MarkerData *data = NULL;
Animation *animation = NULL;
ModelFitter *fitter = NULL;
string fitter_method = "sugihara";
bool analyze_mode = false;
unsigned int max_steps = 100;

void print_usage(const char* execname) {
	cout << "Usage: " << execname << " <modelfile.lua> <mocapdata.c3d> [motion.csv] [--levenberg] [-s count]" << endl;
	cout << "-s count    : sets the maximum number of IK steps to count (default 200)." << endl;
	cout << "" << endl;
	cout << "Note: when specifying motion file no inverse kinematics is performed. Instead it" << endl
		<< "analyzes the the motion file and saves the result to the file fitting_log.csv" << endl;
}

bool parse_args (int argc, char* argv[]) {
	for (int i = 1; i < argc; i++) {
		std::string arg (argv[i]);
		if ((arg == "-s") && (argc > i)) {
			istringstream convert (argv[i + 1]);
			if (!(convert >> max_steps)) {
				cerr << "Error: cannot parse number argument of -s: " << argv[i+1] << endl;
				return false;
			}
			i++;
			continue;
		} else if (arg.substr(arg.size() - 4, 4) == ".lua") {
			model = new Model();
			if (!model->loadFromFile (arg.c_str()))
				return false;
		} else if (arg.substr(arg.size() - 4, 4) == ".c3d") {
			data = new MarkerData();
			if(!data->loadFromFile (arg.c_str())) 
				return false;
		} else if (arg.substr(arg.size() - 4, 4) == ".csv") {
			analyze_mode = true;
			animation = new Animation();
			animation->loadFromFile (arg.c_str());
		} else if (arg == "--levenberg") {
			fitter_method = "levenberg";
		} else if (arg == "--sugiharats") {
			fitter_method = "sugiharats";
		} else {
			return false;
		}
	}

	return true;
}

int main (int argc, char* argv[]) {
	parse_args (argc, argv);

	if (!model || !data)
		print_usage(argv[0]);

	if (fitter_method == "sugihara") {
		fitter = new SugiharaFitter(model, data, max_steps);
	} else if (fitter_method == "sugiharats") {
		fitter = new SugiharaTaskSpaceFitter(model, data, max_steps);
	} else {
		fitter = new LevenbergMarquardtFitter (model, data, max_steps);
	}

	if (analyze_mode) {
		fitter->analyzeAnimation (*animation);
		return 0;
	}

	animation = new Animation();

	TimerInfo timer;

	timer_start(&timer);
	bool result = fitter->computeModelAnimationFromMarkers (model->modelStateQ, animation, data->getFirstFrame(), data->getLastFrame());
	cout << "Duration: " << timer_stop(&timer) << endl;

	if (!result) {
		cout << "Fit failed!" << endl;
	} else {
		cout << "Fit successful!" << endl;
	}
	animation->saveToFile ("animation.csv");

	delete fitter;
	delete animation;
	delete model;
	delete data;

	return result;
}
