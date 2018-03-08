#include "SQueue.hpp"

/* Function to create Circular queue */
bool Queue::enQueue(float value)
{
	if ((front == 0 && rear == size - 1) ||
		(rear == front - 1))
	{
		//printf("\nQueue is Full");
		return false;
	}

	else if (front == -1) /* Insert First Element */
	{
		front = rear = 0;
		arr[rear] = value;
	}

	else if (rear == size - 1 && front != 0)
	{
		rear = 0;
		arr[rear] = value;
	}

	else
	{
		rear++;
		arr[rear] = value;
	}
	return true;
}

// Function to delete element from Circular Queue
int Queue::deQueue()
{
	if (front == -1)
	{
		//printf("\nQueue is Empty");
		return -1;
	}

	int data = arr[front];
	arr[front] = -1;
	if (front == rear)
	{
		front = -1;
		rear = -1;
	}
	else if (front == size - 1)
		front = 0;
	else
		front++;

	return data;
}

// Function displaying the elements
// of Circular Queue
void Queue::displayQueue()
{
	if (front == -1)
	{
		printf("\nQueue is Empty");
		return;
	}
	printf("\nElements in Circular Queue are: ");
	if (rear >= front)
	{
		for (int i = front; i <= rear; i++)
			printf("%f ", arr[i]);
	}
	else
	{
		for (int i = front; i < size; i++)
			printf("%f ", arr[i]);

		for (int i = 0; i <= rear; i++)
			printf("%f ", arr[i]);
	}
}

/*
Source for finding median.
Found on: http://www.cplusplus.com/forum/general/30804/
By: filipe
*/
float Queue::getMedian()
{
	int size = sizeof(arr) / sizeof(float);
	std::sort(&arr[0], &arr[size]);
	double median = size % 2 ? arr[size / 2] : (arr[size / 2 - 1] + arr[size / 2]) / 2;
	return median;
}

