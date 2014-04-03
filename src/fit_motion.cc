#include <iostream>

#include "timer.h"

#include "MarkerModel.h"
#include "MarkerData.h"
#include "Animation.h"
#include "ModelFitter.h"

using namespace std;

MarkerModel *model = NULL;
MarkerData *data = NULL;
Animation *animation = NULL;
ModelFitter *fitter = NULL;
string fitter_method = "sugihara";
bool analyze_mode = false;

void print_usage(const char* execname) {
	cout << "Usage: " << execname << " <modelfile.lua> <mocapdata.c3d> [motion.csv] [--levenberg]" << endl;
	cout << "Note: when specifying motion file no inverse kinematics is performed. Instead it" << endl
		<< "analyzes the the motion file and saves the result to the file fitting_log.csv" << endl;
}

bool parse_args (int argc, char* argv[]) {
	for (int i = 1; i < argc; i++) {
		std::string arg (argv[i]);
		if (arg.substr(arg.size() - 4, 4) == ".lua") {
			model = new MarkerModel();
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
			print_usage (argv[0]);
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
		fitter = new SugiharaFitter(model, data);
	} else if (fitter_method == "sugiharats") {
		fitter = new SugiharaTaskSpaceFitter(model, data);
	} else {
		fitter = new LevenbergMarquardtFitter (model, data);
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
