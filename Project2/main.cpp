#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>

#include <thread>
#include <stdlib.h>

#include "CSkeletonTracking.hpp"

#include "SocketTCP.hpp"

//#include "SQueue.hpp"

using namespace std;

//Main object in this whole project :D
CSkeletonTracking st;

//Used for GLUT window dimensions.
double w;
double h;

//Paints canvas (X-Y pairs of each joint point including lines between them)
void Draw()
{
	glClear(GL_COLOR_BUFFER_BIT); // clear display window

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPointSize(5.0f);
	glBegin(GL_POINTS);
		for (int i = 0; i < 6; i++)
		{
			switch (i)
			{
			case 0:
				//left hand
				glColor3f(0.0, 1.0, 0.0);
				glVertex2i(st.handLeftMedianX, h - st.handLeftMedianY);
				break;
			case 1:
				//right hand
				glColor3f(0.0, 0.5, 0.0);
				glVertex2i(st.handRightMedianX, h - st.handRightMedianY);
				break;
			case 2:
				//glava
				glColor3f(1.0, 1.0, 1.0);
				glVertex2i(st.headMedianX, h - st.headMedianY);
				break;
			case 3:
				//spine-mid
				glColor3f(0.3, 0.3, 0.3);
				glVertex2i(st.spineMedianX, h - st.spineMedianY);
			case 4:
				//left shoulder
				glColor3f(0.0, 0.0, 1.0);
				glVertex2i(st.shoulderLeftMedianX, h - st.shoulderLeftMedianY);
				break;
			case 5:
				//right shoulder
				glColor3f(0.0, 0.0, 0.5);
				glVertex2i(st.shoulderRightMedianX, h - st.shoulderRightMedianY);
				break;
			}			
		}
	glEnd();

	glColor3f(1.0, 1.0, 1.0);

	glBegin(GL_LINES);
		glVertex2f(st.headMedianX, h-st.headMedianY);
		glVertex2f(st.spineMedianX, h-st.spineMedianY);
	glEnd();

	glBegin(GL_LINES);
		glVertex2f(st.spineMedianX, h-st.spineMedianY);
		glVertex2f(st.shoulderLeftMedianX, h-st.shoulderLeftMedianY);
	glEnd();

	glBegin(GL_LINES);
		glVertex2f(st.shoulderRightMedianX, h-st.shoulderRightMedianY);
		glVertex2f(st.spineMedianX, h-st.spineMedianY);
	glEnd();

	glBegin(GL_LINES);
		glVertex2f(st.shoulderLeftMedianX, h-st.shoulderLeftMedianY);
		glVertex2f(st.handLeftMedianX, h-st.handLeftMedianY);
	glEnd();

	glBegin(GL_LINES);
		glVertex2f(st.shoulderRightMedianX, h-st.shoulderRightMedianY);
		glVertex2f(st.handRightMedianX, h-st.handRightMedianY);
	glEnd();
	
	glutSwapBuffers();
}

//Timer is set to repaint canvas every x ms (in this case 20)
void Timer(int iUnused)
{
	//If skeleton tracking is not feeling good, terminate thread
	if (!st.active)
	{
		std::terminate();
	}
	glutPostRedisplay();
	glutTimerFunc(20, Timer, 0);
}

//Initializes canvas (clears it and sets projection)
void Initialize()
{	
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	w = glutGet(GLUT_WINDOW_WIDTH);
	h = glutGet(GLUT_WINDOW_HEIGHT);

	gluOrtho2D(0.0, w, 0.0, h);
}

//Initializes GLUT window, display functions, and other variavles needed for canvas (OpenGL) to work
void setDrawing(int *iArgc, char **cppArgv)
{
	glutInit(iArgc, cppArgv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(958, 718);
	glutInitWindowPosition(350, 10);
	glutCreateWindow("Skeleton Tracking");
	Initialize();
	glutDisplayFunc(Draw);
	Timer(0);
	glutMainLoop();
}
bool sendPoint(SOCKET sock, float point, char* jointName = "NONE")
{
	char strPoint[256];

	sprintf(strPoint, "%s %.2f", jointName, point);

	// Send the text
	int sendResult = send(sock, strPoint, sizeof(strPoint), 0);
	if (sendResult == SOCKET_ERROR)
	{
		// Wait for response 
		//ZeroMemory(buf, 4096);
		//int bytesRecieved = recv(sock, buf, 4096, 0);
		//if (bytesRecieved > 0)
		//{
		//	// Echo response to console
		//	cout << "SERVER> " << string(buf, 0, bytesRecieved) << endl;
		//}
		return false;
	}
	_sleep(1);
	return true;
}

void sendData()
{
	//wait for tracking to kick in
	Sleep(3000);

	string ipAddress = "127.0.0.1";     // IP Address of the server (localhost)
	int port = 54000;           // Listening port # on the server

								// Initialize WinSock
	WSADATA data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		cerr << "Can't start Winsock, Err # " << wsResult << endl;
		return;
	}

	// Create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cerr << "Can't create socket, Err # " << WSAGetLastError() << endl;
		return;
	}

	// Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	// Connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		cerr << "Can't connect to server, Err # " << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return;
	}

	// Do-while loop to send and recieve data
	//char buf[4096];
	//string userInput;

	do
	{
		if (!sendPoint(sock, st.handLeftMedianX, "JOINT_HAND_LEFT_X"))
		{
			break;
		}
		if (!sendPoint(sock, st.handLeftMedianY, "JOINT_HAND_LEFT_Y"))
		{
			break;
		}
		if (!sendPoint(sock, st.handRightMedianX, "JOINT_HAND_RIGHT_X"))
		{
			break;
		}
		if (!sendPoint(sock, st.handRightMedianY, "JOINT_HAND_RIGHT_Y"))
		{
			break;
		}
		if (!sendPoint(sock, st.headMedianX, "JOINT_HEAD_X"))
		{
			break;
		}
		if (!sendPoint(sock, st.headMedianY, "JOINT_HEAD_Y"))
		{
			break;
		}
		if (!sendPoint(sock, st.spineMedianX, "JOINT_SPINE_MID_X"))
		{
			break;
		}
		if (!sendPoint(sock, st.spineMedianY, "JOINT_SPINE_MID_Y"))
		{
			break;
		}
		if (!sendPoint(sock, st.shoulderLeftMedianX, "JOINT_SHOULDER_LEFT_X"))
		{
			break;
		}
		if (!sendPoint(sock, st.shoulderLeftMedianY, "JOINT_SHOULDER_LEFT_Y"))
		{
			break;
		}
		if (!sendPoint(sock, st.shoulderRightMedianX, "JOINT_SHOULDER_RIGHT_X"))
		{
			break;
		}
		if (!sendPoint(sock, st.shoulderRightMedianY, "JOINT_SHOULDER_RIGHT_Y"))
		{
			break;
		}

	} while (st.active);
	// Gracefully close down everything
	closesocket(sock);
	WSACleanup();
}

void main(int iArgc, char** cppArgv)
{
	//Initialization of our main object.
	st = CSkeletonTracking();

	//Opens new thread with canvas because otherwise method CSkeletonTracking::Update() will block drawing
	std::thread t1(setDrawing, &iArgc, cppArgv);

	std::thread t2(sendData);

	/*std::thread t2(&CSkeletonTracking::Update, std::ref(st));*/

	st.Update();
	
	cout << "Exited update thread. Stopped tracking.\n";

	//Wait for thread to finish.
	t1.join();
	t2.join();
	
	return;
}
