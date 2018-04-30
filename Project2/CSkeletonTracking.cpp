#include "CSkeletonTracking.hpp"

CSkeletonTracking::CSkeletonTracking() :
	handLeftX(Queue(QUEUE_SIZE)),		handLeftY(Queue(QUEUE_SIZE)),	handLeftZ(Queue(QUEUE_SIZE)),
	handRightX(Queue(QUEUE_SIZE)),		handRightY(Queue(QUEUE_SIZE)),	handRightZ(Queue(QUEUE_SIZE)),
	headX(Queue(QUEUE_SIZE)),			headY(Queue(QUEUE_SIZE)),
	spineX(Queue(QUEUE_SIZE)),			spineY(Queue(QUEUE_SIZE)),
	shoulderLeftX(Queue(QUEUE_SIZE)),	shoulderLeftY(Queue(QUEUE_SIZE)),
	shoulderRightX(Queue(QUEUE_SIZE)),	shoulderRightY(Queue(QUEUE_SIZE)),
	started(false),
	active(true),
	handLeftMedianX(12.32),		handLeftMedianY(0.0),
	handRightMedianX(0.0),		handRightMedianY(0.0),
	headMedianX(0.0),			headMedianY(0.0),
	spineMedianX(0.0),			spineMedianY(0.0),
	shoulderLeftMedianX(0.0),	shoulderLeftMedianY(0.0),
	shoulderRightMedianX(0.0),	shoulderRightMedianY(0.0)
{
	configureCamera();
}

void CSkeletonTracking::configureCamera()
{
	try
	{
		pxcStatus status;
		//Uvijek treba imati instancu sessiona
		session = PXCSession::CreateInstance();
		sm = session->CreateSenseManager();

		status = sm->EnableStream(PXCCapture::StreamType::STREAM_TYPE_COLOR, 320, 240, 30);

		status = sm->EnablePersonTracking();

		status = sm->Init();

		trackingModule = sm->QueryPersonTacking();

		trackingConfig = trackingModule->QueryConfiguration();
		trackingConfig->SetTrackedAngles(PXCPersonTrackingConfiguration::TrackingAngles::TRACKING_ANGLES_ALL);

		jointsConfig = trackingConfig->QuerySkeletonJoints();
		jointsConfig->Enable();
		jointsConfig->SetTrackingArea(PXCPersonTrackingConfiguration::SkeletonJointsConfiguration::SkeletonMode::AREA_UPPER_BODY);
		jointsConfig->SetMaxTrackedPersons(1);
	}
	catch (int param)
	{
		std::cout << "Error";
		//active = false;
		return;
	}
}

void CSkeletonTracking::Update()
{
	if (NULL == sm)
	{
		std::cout << "Sense manager is null, CSkeletonTracking.cpp - line 53\n";
		//active = false;
	}

	while (sm->AcquireFrame(true) >= pxcStatus::PXC_STATUS_NO_ERROR)
	{
		try
		{
			if (!started)
			{
				std::cout << "Sucessfully started tracking\n";
				started = true;
			}


			PXCCapture::Sample *sample = sm->QuerySample();
			trackingData = trackingModule->QueryOutput();

			int numOfPersons = trackingData->QueryNumberOfPeople();

			for (int i = 0; i < numOfPersons; i++)
			{
				trackingData->StartTracking(i);
				PXCPersonTrackingData::Person *trackedPerson = trackingData->QueryPersonData(PXCPersonTrackingData::AccessOrderType::ACCESS_ORDER_BY_ID, i);

				PXCPersonTrackingData::PersonJoints *personJoints = trackedPerson->QuerySkeletonJoints();

				int njoints = personJoints->QueryNumJoints();

				PXCPersonTrackingData::PersonJoints::SkeletonPoint *points = new PXCPersonTrackingData::PersonJoints::SkeletonPoint[njoints];


				personJoints->QueryJoints(points);

				for (int i = 0; i < njoints; i++)
				{
					float xCoo = ((-1 * points[i].image.x) + xNULL);
					float yCoo = (points[i].image.y + yNULL);
					
					if (xCoo  == xNULL || yCoo == yNULL)
					{
						continue;
					}

					switch (i)
					{
					case 0:
						saveNewPoint(&handLeftX, xCoo);
						saveNewPoint(&handLeftY, yCoo);

						handLeftMedianX = handLeftX.getMedian();
						handLeftMedianY = handLeftY.getMedian();

						continue;
					case 1:
						saveNewPoint(&handRightX, xCoo);
						saveNewPoint(&handRightY, yCoo);
							
						handRightMedianX = handRightX.getMedian();
						handRightMedianY = handRightY.getMedian();
							
						continue;
					case 2:
						saveNewPoint(&headX, xCoo);
						saveNewPoint(&headY, yCoo);

						headMedianX = headX.getMedian();
						headMedianY = headY.getMedian();

						continue;
					case 3:
						saveNewPoint(&spineX, xCoo);
						saveNewPoint(&spineY, yCoo);

						spineMedianX = spineX.getMedian();
						spineMedianY = spineY.getMedian();

						continue;
					case 4:
						saveNewPoint(&shoulderLeftX, xCoo);
						saveNewPoint(&shoulderLeftY, yCoo);

						shoulderLeftMedianX = shoulderLeftX.getMedian();
						shoulderLeftMedianY = shoulderLeftY.getMedian();

						continue;
					case 5:
						saveNewPoint(&shoulderRightX, xCoo);
						saveNewPoint(&shoulderRightY, yCoo);

						shoulderRightMedianX = shoulderRightX.getMedian();
						shoulderRightMedianY = shoulderRightY.getMedian();

						continue;
					default:
						continue;
					}
				}
				delete[] points;
			}
			sm->ReleaseFrame();
		}
		catch (int param)
		{
			std::cout << "Error occured, restart program and the camera\n";
			active = false;
			return;
		}
	}
	std::cout << "Can't catch frames, restart camera/program\n";
	//active = false;
}

void CSkeletonTracking::saveNewPoint(Queue *q, float newPoint)
{
	if (!q->enQueue(newPoint))
	{
		q->deQueue();
		q->enQueue(newPoint);
	}
}
