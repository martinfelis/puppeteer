#include <UnitTest++.h>

#include "SimpleMath/SimpleMathGL.h"
#include "Animation.h"

#include <iostream>

using namespace std;

using namespace SimpleMath;
using namespace SimpleMath::GL;

const float TEST_PREC = 1.0e-6;

TEST ( TestAnimationAddPose ) {
	Animation animation;

	VectorNd pose0 (6);
	pose0 << 0., 0., 0., 0., 0., 0.;

	animation.addPose (0, pose0);

	CHECK_EQUAL (1, animation.keyFrames.size());
	CHECK_EQUAL (0., animation.keyFrames[0].time);	
	CHECK_EQUAL (pose0, animation.keyFrames[0].state);	
}

TEST ( TestAnimationAddTwoPoses ) {
	Animation animation;

	VectorNd pose0 (6);
	pose0 << 0., 0., 0., 0., 0., 0.;

	VectorNd pose1 (6);
	pose1 << 1., 1., 1., 1., 1., 1.;

	animation.addPose (0., pose0);
	animation.addPose (1., pose1);

	CHECK_EQUAL (2, animation.keyFrames.size());
	CHECK_EQUAL (0., animation.keyFrames[0].time);	
	CHECK_EQUAL (pose0, animation.keyFrames[0].state);	

	CHECK_EQUAL (1., animation.keyFrames[1].time);	
	CHECK_EQUAL (pose1, animation.keyFrames[1].state);	
}

TEST ( TestAnimationAddThreePoses ) {
	Animation animation;

	VectorNd pose0 (6);
	pose0 << 0., 0., 0., 0., 0., 0.;

	VectorNd pose1 (6);
	pose1 << 1., 1., 1., 1., 1., 1.;

	VectorNd pose_5 (6);
	pose_5 << 0.5, 0.5, 0.5, 0.5, 0.5, 0.5;

	animation.addPose (0., pose0);
	animation.addPose (1., pose1);
	animation.addPose (.5, pose_5);

	CHECK_EQUAL (3, animation.keyFrames.size());
	CHECK_EQUAL (0., animation.keyFrames[0].time);	
	CHECK_EQUAL (pose0, animation.keyFrames[0].state);	

	CHECK_EQUAL (0.5, animation.keyFrames[1].time);	
	CHECK_EQUAL (pose_5, animation.keyFrames[1].state);	

	CHECK_EQUAL (1., animation.keyFrames[2].time);	
	CHECK_EQUAL (pose1, animation.keyFrames[2].state);	
}

TEST ( TestAnimationGetInterpolated ) {
	Animation animation;

	VectorNd pose0 (6);
	pose0 << 0., 0., 0., 0., 0., 0.;

	VectorNd pose1 (6);
	pose1 << 1., 1., 1., 1., 1., 1.;

	animation.addPose (0., pose0);
	animation.addPose (1., pose1);

	CHECK_EQUAL (2, animation.keyFrames.size());

	animation.setCurrentTime (-1.);
	CHECK_EQUAL (0., animation.currentTime);
	VectorNd pose = animation.getCurrentPose();
	CHECK_EQUAL (pose0, pose);

	animation.setCurrentTime (2.);
	CHECK_EQUAL (1., animation.currentTime);
	pose = animation.getCurrentPose();
	CHECK_EQUAL (pose1, pose);

	VectorNd pose_5 (6);
	pose_5 << 0.5, 0.5, 0.5, 0.5, 0.5, 0.5;

	animation.setCurrentTime (0.5);
	CHECK_EQUAL (0.5, animation.currentTime);
	pose = animation.getCurrentPose();
	CHECK_EQUAL (pose_5, pose);
}
