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

#ifndef MODEL_FITTER_H
#define MODEL_FITTER_H

#include "SimpleMath/SimpleMath.h"

struct MarkerData;
struct Model;
struct Animation;

struct ModelFitter {
	struct ModelFitterInternal;

	MarkerData *data;
	Model *model;
	ModelFitterInternal *internal;

	double tolerance;
	bool success;
	unsigned int steps;
	unsigned int maxSteps;

	VectorNd initialState;
	VectorNd fittedState;
	VectorNd residuals;

	ModelFitter ();
	ModelFitter (Model *model, MarkerData *data, unsigned int maxSteps);
	virtual ~ModelFitter();

	void setup();
	virtual bool run (const VectorNd &initialState) = 0;

	bool computeModelAnimationFromMarkers (const VectorNd &initialState, Animation *animation, int frame_start = -1, int frame_end = -1);
	void analyzeAnimation (Animation animation);

	VectorNd getFittedState() {
		return fittedState;
	}
};

struct SugiharaFitter : public ModelFitter {
	SugiharaFitter (Model *model, MarkerData *data, unsigned int maxSteps = 200) :
		ModelFitter (model, data, maxSteps) 
	{}
	virtual ~SugiharaFitter() {};
	virtual bool run (const VectorNd &initialState);
};

struct SugiharaTaskSpaceFitter : public ModelFitter {
	SugiharaTaskSpaceFitter (Model *model, MarkerData *data, unsigned int maxSteps = 200) :
		ModelFitter (model, data, maxSteps) 
	{}
	virtual ~SugiharaTaskSpaceFitter() {};
	virtual bool run (const VectorNd &initialState);
};

struct LevenbergMarquardtFitter : public ModelFitter {
	LevenbergMarquardtFitter (Model *model, MarkerData *data, unsigned int maxSteps = 200, double lambda = 0.05):
		ModelFitter (model, data, maxSteps),
		lambda (lambda)
	{}
	virtual ~LevenbergMarquardtFitter() {};
	virtual bool run (const VectorNd &initialState);

	double lambda;
};

/* MODEL_FITTER_H */
#endif 
