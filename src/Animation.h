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
	Animation() :
		currentTime (0.)
	{}
	double currentTime;
	std::vector<AnimationKeyFrame> keyFrames;

	void addPose (double time, const VectorNd &states);
	void setCurrentTime (double time);
	VectorNd getCurrentPose () const;

	double getFirstFrameTime() const;
	double getLastFrameTime() const;
	double getDuration() const { return getLastFrameTime() - getFirstFrameTime(); };

	bool loadFromFile (const char* filename);
	void saveToFile (const char* filename) const;
  
  const VectorNd getTimeLine() const;
  const VectorNd getStateLine(const size_t _stateIdx) const;
};

/* ANIMATION_H */
#endif 
