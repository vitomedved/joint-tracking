#pragma once

// Queue struct source code for taken from: 
// https://www.geeksforgeeks.org/circular-queue-set-1-introduction-array-implementation/
// Finding median added manualy to this source code

// C or C++ program for insertion and
// deletion in Circular Queue
//#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

struct Queue
{
	// Initialize front and rear
	int rear, front;

	// Circular Queue
	int size;
	float *arr;

	Queue(int s)
	{
		front = rear = -1;
		size = s;
		arr = new float[s];
	}

	bool enQueue(float value);
	int deQueue();
	void displayQueue();

	float getMedian();
};