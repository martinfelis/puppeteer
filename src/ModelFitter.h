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
	~ModelFitter();

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
	virtual bool run (const VectorNd &initialState);
};

struct SugiharaTaskSpaceFitter : public ModelFitter {
	SugiharaTaskSpaceFitter (Model *model, MarkerData *data, unsigned int maxSteps = 200) :
		ModelFitter (model, data, maxSteps) 
	{}
	virtual bool run (const VectorNd &initialState);
};

struct LevenbergMarquardtFitter : public ModelFitter {
	LevenbergMarquardtFitter (Model *model, MarkerData *data, unsigned int maxSteps = 200, double lambda = 0.05):
		ModelFitter (model, data, maxSteps),
		lambda (lambda)
	{}
	double lambda;

	virtual bool run (const VectorNd &initialState);
};

/* MODEL_FITTER_H */
#endif 
