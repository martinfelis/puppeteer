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

void print_usage(const char* execname) {
	cout << "Usage: " << execname << " <modelfile.lua> <mocapdata.c3d> [--levenberg]" << endl;
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
		} else if (arg == "--levenberg") {
			fitter_method = "levenberg";
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
	} else {
		fitter = new LevenbergMarquardtFitter (model, data);
	}

	animation = new Animation();

	TimerInfo timer;

	timer_start(&timer);
	bool result = fitter->computeModelAnimationFromMarkers (model->modelStateQ, animation, data->getFirstFrame(), data->getLastFrame() - 1);
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
