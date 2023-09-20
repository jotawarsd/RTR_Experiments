#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

struct Node
{
	int data;
	Node *next;
};

Node *top;

int main(void)
{
	//function prototypes
	void push(int data);
	int isEmpty(void);
	int getTop(void);
	void pop(void);
	void display(void);

	//variable declarations
	int ch;
	int data = 0;

	//code
	do
	{
		cout << "\n1. Push to Stack\n2. Pop from Stack\n3. Print top Element\n4. Print All Elements\n5. Exit";
		cout << "\nEnter Choice: ";
		cin >> ch;
		cout << "\n";

		switch (ch)
		{
		case 1:
			cout << "Enter Data to Push: ";
			cin >> data;
			push(data);
			break;
		case 2:
			pop();
			break;
		case 3:
			data = getTop();
			cout << "Top of the stack: " << data << endl;
			break;
		case 4:
			display();
			break;
		default:
			break;
		}
	} while (ch < 5 && ch >= 1);

	if (top != NULL)
	{
		Node *temp = new Node();
		temp = top;
		while (top->next != NULL)
		{
			temp = top->next;
			top = temp;
			free(temp);
			temp = NULL
		}
		temp = NULL;
		top = NULL;
	}
}

void push(int data)
{
	Node *temp = new Node();

	temp->data = data;
	temp->next = top;

	top = temp;
}

int isEmpty(void)
{
	return top == NULL;
}

int getTop(void)
{
	//function prototypes
	int isEmpty(void);

	//code
	if (!isEmpty())
		return top->data;
	else
		return 0;
}

void pop(void)
{
	Node *temp;

	if (top == NULL)
	{
		cout << "Stack Overflow!!" << endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		temp = top;

		cout << "Popped Data: " << temp->data << endl;
		top = temp->next;

		free(temp);
		temp = NULL;
	}
}

void display(void)
{
	Node *temp;

	if (top == NULL)
	{
		cout << "Stack Underflow" << endl;
		exit(1);
	}	
	else
	{
		temp = top;
		while (temp != NULL)
		{
			cout << temp->data << " | ";
			temp = temp->next;
		}
	}
}
