#ifndef MODEL_FITTER_H
#define MODEL_FITTER_H

#include "SimpleMath/SimpleMath.h"

struct MarkerData;
struct MarkerModel;
struct Animation;

struct ModelFitter {
	struct ModelFitterInternal;

	MarkerData *data;
	MarkerModel *model;
	ModelFitterInternal *internal;

	double tolerance;
	bool success;
	unsigned int steps;

	VectorNd initialState;
	VectorNd fittedState;
	VectorNd residuals;

	ModelFitter ();
	ModelFitter (MarkerModel *model, MarkerData *data);
	~ModelFitter();

	void setup();
	virtual bool run (const VectorNd &initialState) = 0;

	bool computeModelAnimationFromMarkers (const VectorNd &initialState, Animation *animation, int frame_start = -1, int frame_end = -1);

	VectorNd getFittedState() {
		return fittedState;
	}
};

struct SugiharaFitter : public ModelFitter {
	SugiharaFitter (MarkerModel *model, MarkerData *data) :
		ModelFitter (model, data) 
	{}
	virtual bool run (const VectorNd &initialState);
};

struct SugiharaTaskSpaceFitter : public ModelFitter {
	SugiharaTaskSpaceFitter (MarkerModel *model, MarkerData *data) :
		ModelFitter (model, data) 
	{}
	virtual bool run (const VectorNd &initialState);
};

struct LevenbergMarquardtFitter : public ModelFitter {
	LevenbergMarquardtFitter (MarkerModel *model, MarkerData *data, double lambda = 0.05):
		ModelFitter (model, data),
		lambda (lambda)
	{}
	double lambda;

	virtual bool run (const VectorNd &initialState);
};

/* MODEL_FITTER_H */
#endif 
