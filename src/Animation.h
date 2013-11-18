#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>

#include "SimpleMath/SimpleMath.h"

struct AnimationKeyFrame {
	AnimationKeyFrame (double _time, const VectorNd _state) :
		time (_time),
		state (_state) {}

	bool operator== (const AnimationKeyFrame& keyframe) const {
		return (time == keyframe.time && state == keyframe.state);
	}

	double time;
	VectorNd state;
};

struct Animation {
	double currentTime;
	std::vector<AnimationKeyFrame> keyFrames;

	void addPose (double time, const VectorNd &states);
	void setCurrentTime (double time);
	VectorNd getCurrentPose ();

	double getFirstFrameTime();
	double getLastFrameTime();
	double getDuration() { return getLastFrameTime() - getFirstFrameTime(); };

	bool loadFromFile (const char* filename);
	void saveToFile (const char* filename);
};

/* ANIMATION_H */
#endif 
