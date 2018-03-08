#pragma once

#include "pxcsession.h"
#include "pxcsensemanager.h"
#include "pxcfacedata.h"
#include "pxcpersontrackingmodule.h"
#include "pxcpersontrackingconfiguration.h"

//Used for circural queue
#include "SQueue.hpp"

//Used for cout
#include <iostream>

//These will be added to x and y coordinates to
//get more spread-out points for OpenGL drawing 
//(set to 0 if you don't need this)
#define xNULL 479
#define yNULL 359

//Defines size of each point's queue (check line 67)
#define QUEUE_SIZE 7

//SkeletonTracking class - implements Intel Realsense SDK for R200 camera
//(Older version of SDK)
class CSkeletonTracking
{

public:
	//Constructor will try to initialize camera automatically, allocate queues, and some variables
	CSkeletonTracking();
	//Tries to initialize camera and module (pipeline) for Skeleton tracking.
	//Current settings allows for only 1 person's joints to be tracked.
	void configureCamera();
	//Main method, it's called in another thread so it doesn't block the process.
	//Inside there is basically a while loop which will try to acquire frames from camera, and analize frames
	//to try and find skeleton joints on 1 person.
	void Update();
	//Saves a new point in circural queue (if queue is full, it will dequeue last value, and add new one to the start of the queue)
	void saveNewPoint(Queue *q, float newPoint);

	//Currently not used arrays
	//float m_x[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	//float m_y[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

	//Public variables of current X and Y coordinates (median out of 7 points)
	float handLeftMedianX, handLeftMedianY;
	float handRightMedianX, handRightMedianY;
	float headMedianX, headMedianY;
	float spineMedianX, spineMedianY;
	float shoulderLeftMedianX, shoulderLeftMedianY;
	float shoulderRightMedianX, shoulderRightMedianY;

private:
	//Used for error detection
	pxcStatus status;
	
	//SDK variables - check: https://software.intel.com/sites/landingpage/realsense/camera-sdk/v1.1/documentation/html/index.html?doc_devguide_introduction.html
	//for all info about PXC classes
	PXCSession *session;
	PXCSenseManager *sm;
	PXCFaceData *faceData;
	PXCPersonTrackingData *trackingData;
	PXCPersonTrackingModule *trackingModule;
	PXCPersonTrackingConfiguration *trackingConfig;
	PXCPersonTrackingConfiguration::SkeletonJointsConfiguration *jointsConfig;
	
	//Queue variables used to determine median position of each point (currently queue size is 7)
	//Z coordinates still not implemented
	Queue handLeftX;
	Queue handLeftY;
	Queue handLeftZ;

	Queue handRightX;
	Queue handRightY;
	Queue handRightZ;

	Queue headX;
	Queue headY;

	Queue spineX;
	Queue spineY;

	Queue shoulderLeftX;
	Queue shoulderLeftY;

	Queue shoulderRightX;
	Queue shoulderRightY;

	//Local variable used as a flag to write whether tracking has successfully started.
	bool started;

	//Array of joint names (currently not used, indexes determined by IRS SDK)
	char *jointName[21] =
	{
		"",
		"",
		"",
		"",
		"",
		"",
		"HAND_LEFT",
		"HAND_RIGHT",
		"",
		"",
		"HEAD",
		"",
		"",
		"",
		"",
		"",
		"SHOULDER_LEFT",
		"SHOULDER_RIGHT",
		"",
		"SPINE_MID",
		""
	};
};
