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


void main(int iArgc, char** cppArgv)
{
	
	//server side
	//main locals
	WSAData WinSockData;
	WORD DLLVERSION;

	//konkatenacija 2 i 2 za verziju dll-a
	DLLVERSION = MAKEWORD(2, 2);

	//pokrecem winsock dll
	int wsOk = WSAStartup(DLLVERSION, &WinSockData);
	if (wsOk != 0)
	{
		cerr << "Can't init winsock! Exiting..." << endl;
		return;
	}

	SOCKET sock_LISTEN = socket(AF_INET, SOCK_STREAM, 0);;
	if (sock_LISTEN == INVALID_SOCKET)
	{
		cerr << "Can't create listen socket...";
		return;
	}


	sockaddr_in ADDRESS;
	int AddressSize = sizeof(ADDRESS);
	ADDRESS.sin_family = AF_INET;
	ADDRESS.sin_port = htons(55000);
	ADDRESS.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//INADDR_ANY;//


	bind(sock_LISTEN, (sockaddr*)&ADDRESS, sizeof(ADDRESS));
	listen(sock_LISTEN, SOMAXCONN);

	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET sock_CLIENT = accept(sock_LISTEN, (sockaddr*)&client, &clientSize);

	/*char host[NI_MAXHOST];
	char service[NI_MAXHOST];

	memset(host, 0, NI_MAXHOST);
	memset(service, 0, NI_MAXHOST);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << " connected on port " << service << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " << ntohs(client.sin_port) << endl;
	}

	closesocket(sock_LISTEN);

	char buff[4096];

	while (true)
	{
		memset(buff, 0, 4096);

		int bytesRecv = recv(sock_CLIENT, buff, 4096, 0);
		if (bytesRecv == SOCKET_ERROR)
		{
			cerr << "Error with recv()." << endl;
			break;
		}

		if (bytesRecv == 0)
		{
			cout << "Client disconnected!";
			break;
		}

		send(sock_CLIENT, buff, bytesRecv + 1, 0);

	}
	closesocket(sock_CLIENT);
	WSACleanup();
*/
	
	sock_CLIENT = socket(AF_INET, SOCK_STREAM, 0);

	if (sock_CLIENT == INVALID_SOCKET)
	{
		return;
	}


	cout << "Waiting for incomming connection...\n";

	int size = sizeof(ADDRESS);
	
	//for (;;)
	//{

	//	if ((sock_CLIENT = accept(sock_LISTEN, (SOCKADDR*)&client, &clientSize)) != INVALID_SOCKET)
	//	{
	//		//char host[NI_MAXHOST];		//clients remote name
	//		//char service[NI_MAXHOST];	//service the client is connected to

	//		//memset(host, 0, NI_MAXHOST);
	//		//memset(service, 0, NI_MAXHOST);

	//		cout << "CONNECTION ESTABLISHED\n";
	//		send(sock_CLIENT, "Welcome!\n", 11, NULL);
	//		//SUCCESSFULL = send(sock_CONNECTION, "Welcome!\n", 10, NULL);
	//		
	//		//closesocket(sock_LISTEN);
	//		
	//		return;
	//	}
	//}
	closesocket(sock_CLIENT);
	WSACleanup();
	//Server server = Server();
	//server.Start("456");

	
	//Initialization of our main object.
	st = CSkeletonTracking();

	//Opens new thread with canvas because otherwise method CSkeletonTracking::Update() will block drawing
	std::thread t1(setDrawing, &iArgc, cppArgv);

	/*std::thread t2(&CSkeletonTracking::Update, std::ref(st));*/

	st.Update();
	
	cout << "Exited update thread. Stopped tracking.\n";

	//Wait for thread to finish.
	t1.join();
	//t2.join();
	
	return;
}
