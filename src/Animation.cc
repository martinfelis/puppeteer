#include <assert.h>
#include <fstream>
#include <sstream>

#include "Animation.h"
#include "string_utils.h"

using namespace std;

void Animation::addPose (double time, const VectorNd &state) {
	AnimationKeyFrame keyframe (time, state);

	vector<AnimationKeyFrame>::iterator iter = keyFrames.begin();
	vector<AnimationKeyFrame>::iterator next = iter;
	next++;

	if (iter == keyFrames.end()) {
		keyFrames.push_back (keyframe);
		return;
	}

	do {
		if (next == keyFrames.end()) {
			keyFrames.insert (next, keyframe);
			return;
		} else {
			if (next->time > time) {
				keyFrames.insert (next, keyframe);
				return;
			}
		}
		iter = next;
		next++;
	} while (iter != keyFrames.end());
}

void Animation::setCurrentTime (double time) {
	if (keyFrames.size() == 0) {
		cerr << "Error: cannot set animation time: no keyframes defined" << endl;
		abort();
	}

	if (time < keyFrames[0].time) {
		currentTime = keyFrames[0].time;
		return;
	}

	if (time > keyFrames.rbegin()->time) {
		currentTime = keyFrames.rbegin()->time;
		return;
	}

	currentTime = time;
}

VectorNd Animation::getCurrentPose() const {
	if (keyFrames.size() == 0) {
		cerr << "Error: cannot get pose: no keyframes defined" << endl;
		abort();
	}

	if (keyFrames.size() == 1) {
		return keyFrames[0].state;
	}

	vector<AnimationKeyFrame>::const_iterator iter = keyFrames.begin();
	vector<AnimationKeyFrame>::const_iterator next = iter;
	next++;

	do {
		if (next->time >= currentTime) {
			break;
		}

		iter = next;
		next++;
	} while (next + 1 != keyFrames.end());

	double frac = (currentTime - iter->time) / (next->time - iter->time);
	return (1. - frac) * iter->state + frac * next->state;
}

double Animation::getFirstFrameTime() const {
	if (keyFrames.size() == 0) {
		cerr << "Error: cannot get time: no keyframes defined" << endl;
		abort();
	}

	return keyFrames[0].time;
}

double Animation::getLastFrameTime() const {
	if (keyFrames.size() < 2) {
		cerr << "Error: cannot get time: too few keyframes defined" << endl;
		abort();
	}

	return keyFrames.rbegin()->time;
}

bool Animation::loadFromFile (const char* filename) {
	ifstream infile (filename);
	if (!infile) {
		cerr << "Error reading animation file from '" << filename << "'" << endl;
		abort();
		return false;
	}

	keyFrames.clear();

	int line_index = 0;
	string line;
	string previous_line;

	while (!infile.eof()) {
		previous_line = line;
		getline (infile, line);

		if (infile.eof()) {
			break;
		} else {
			line_index++;
		}

		vector<string> tokens = tokenize_csv_strip_whitespaces (line);
		if (tokens.size() == 0)
			continue;

		double frame_time = 0.;
		VectorNd state = VectorNd::Zero (tokens.size() - 1);

		double value;
		istringstream value_stream (tokens[0]);

		// If the first entry is not a number we ignore the whole line
		if (!(value_stream >> value))
			continue;

		for (size_t i = 0; i < tokens.size(); i++) {
			value_stream.clear();
			value_stream.str(tokens[i]);
			if (!(value_stream >> value)) {
				cerr << "Error: could not convert string '" << tokens[i] << "' to number in " << filename << ", line " << line_index << ", column " << i << endl;
				abort();
			}

			if (i == 0)
				frame_time = value;
			else {
				state[i - 1] = value;
			}
		}
		addPose (frame_time, state);
	}

	infile.close();

	return true;
}

void Animation::saveToFile( const char* filename) const {
	ofstream outfile (filename);

	for (vector<AnimationKeyFrame>::const_iterator iter = keyFrames.begin(); iter != keyFrames.end(); iter++) {
		outfile << iter->time << ", ";
		for (size_t i = 0; i < iter->state.size(); i++) {
			outfile << iter->state[i];
			if (i != iter->state.size() - 1) {
				outfile << ", ";
			}
		}
		outfile << endl;
	}

	outfile.close();
}
