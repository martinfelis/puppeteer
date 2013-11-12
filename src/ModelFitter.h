#ifndef MODEL_FITTER_H
#define MODEL_FITTER_H

#include "SimpleMath/SimpleMath.h"

struct MarkerData;
struct MarkerModel;

struct ModelFitter {
	MarkerData *data;
	MarkerModel *model;
	bool success;
	VectorNd initialState;
	VectorNd fittedState;

	ModelFitter (MarkerModel *model, MarkerData *data):
		data (data),
		model (model),
		success (false)
	{ }

	bool run (const VectorNd &initialState);

	VectorNd getFittedState() {
		return fittedState;
	}
};

/* MODEL_FITTER_H */
#endif 
