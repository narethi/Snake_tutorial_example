//Step 1 add in the compiler handler tags
#ifndef _SNAKE_MAIN_SOURCE
#define _SNAKE_MAIN_SOURCE

//Step 2 add in the needed dependencies
#include <gl\freeglut.h>
#include <stdlib.h>
#include <iostream>

//I know that vectors are depricated, but I like them
#include <vector>

using namespace std;

//The direction enumerator will be used to keep track of where the head
//is travelling
extern enum direction{UP, RIGHT, DOWN, LEFT, STATIONARY};

//goal is an array designed to keep track of the
//x and y coordinates of the target
int goal[2];

//goalColor will determine the color of the goal
//marker
float goalColor[3];

//goalBlink is used to keep track 
//of when to change color
double goalBlink = 0;

//World draw contains all of the information required
//for drawing all the vertices in the world, which is
//100 units by 100 units, with x and y coordinates
int worldDraw[100*100*2];

//World colors contains all of the information needed
//for determining the color of the world map, which is
//100*100 and with Red, Green, Blue parameters
float worldColors[100*100*3];

//This vector will represent the snake, 
//it contains the stack frames for the position
struct snake 
{
	int* segments;
	float* segmentColors;
	int numberOfSegments;
	direction heading;
};

snake roger;

void setupSnake(snake &newSnake)
{
	newSnake.segments = new int[2];
	
	newSnake.segments[0] = 303, 	newSnake.segments[1] = 303;

	newSnake.segmentColors = new float[3];

	newSnake.segmentColors[0] = 0, newSnake.segmentColors[1] = 0,
	newSnake.segmentColors[2] = 1;

	newSnake.numberOfSegments = 1;

	newSnake.heading = STATIONARY;
};

void updateSnakePosition(snake &updatePosition)
{
	//Check if the snake is hitting a wall
	switch(updatePosition.heading)
	{
	case UP: 
		if(updatePosition.segments[1] + 6 > 597) 
			setupSnake(updatePosition);
	case DOWN: 
		if(updatePosition.segments[1] - 6 < 3) 
			setupSnake(updatePosition);
	case LEFT: 
		if(updatePosition.segments[0] - 6 < 3) 
			setupSnake(updatePosition);
	case RIGHT:
		if(updatePosition.segments[0] + 6 > 597) 
			setupSnake(updatePosition);
		break;
	default: ;
	}

	//Move the snake
	if(updatePosition.heading != STATIONARY)
	{
		for(int i = updatePosition.numberOfSegments - 1; i > 0; i--)
		{
			updatePosition.segments[i*2] = updatePosition.segments[i*2 - 2];
			updatePosition.segments[i*2 + 1] = updatePosition.segments[i*2 - 1];
		}
		switch (updatePosition.heading)
		{
			case UP:
				updatePosition.segments[1] = updatePosition.segments[1] + 6;
				break;
			case RIGHT:
				updatePosition.segments[0] = updatePosition.segments[0] + 6;
				break;
			case LEFT:
				updatePosition.segments[0] = updatePosition.segments[0] - 6;
				break;
			case DOWN:
				updatePosition.segments[1] = updatePosition.segments[1] - 6;
				break;
		}
	}
	//Check if the new head of the snake does not collide with the current snake
	for(int i = 1; i < updatePosition.numberOfSegments; i++)
		if(updatePosition.segments[0] == updatePosition.segments[i*2])
			if(updatePosition.segments[1] == updatePosition.segments[i*2 + 1])
				setupSnake(updatePosition);
}

//Part of step 3 you need to set the initial window
int WIDTH = 600;
int HEIGHT = 600;

void randomNumberGenerator(void)
{
	int *temp = new int[1];
	srand((unsigned int)&temp[0]);
	goal[0] = ((rand()%99)*6) + 3;
	srand(rand()%1000);
	goal[1] = ((rand()%99)*6) + 3;
}

//Step 7  initialize all the squares of the grid
void initDrawArrays(void)
{
	randomNumberGenerator();
	goalColor[0] = 1, goalColor[1] = 1, goalColor[2] = 1;
	for(int i = 0; i < 100*100*3; i++) 
		//This sets the initial world color as a .5 red value since
		//only the values at the 0s are set to .5 i.e. 0, 3, 6 ... 3*i
		//Which represent the red channel only
		(i%3 == 0) ? worldColors[i] = .5 : worldColors[i] = 0;
	
	for(int i = 0; i < 100; i++)
	{
		for(int j = 0; j < 100; j++)
		{
			//i*200 moves the array by a full row each iteration
			//of the outerloop, then you add j to increment through
			//the inner part, in this case i is the number of rows
			//and j is the number of columns, i.e. row | column =>
			//i | j, next the second part of the increment is *2
			//since we need to keep track of both x and y.
			//The square size is 6 so we multiply both x and y by
			//the size scalar of 6
			//Finally we are adding padding in so just adding in
			//a straight shift of 1 half square
			worldDraw[i*200 + j*2] = j*6 + 3;
			worldDraw[i*200 + j*2 + 1] = i*6 + 3;
		}
	}
}

void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 'w': roger.heading = UP;
			break;
		case 's': roger.heading = DOWN;
			break;
		case 'a': roger.heading = LEFT;
			break;
		case 'd': roger.heading = RIGHT;
		default: ;
	}
}

snake addBodySegment(snake currentSnake)
{
	snake newSnake;
	currentSnake.numberOfSegments++;

	newSnake.numberOfSegments = currentSnake.numberOfSegments;
	newSnake.segments = new int[newSnake.numberOfSegments*2];
	newSnake.segmentColors = new float[newSnake.numberOfSegments*3];

	newSnake.segments[0] = goal[0], newSnake.segments[1] = goal[1]; 

	for(int i = 1; i < currentSnake.numberOfSegments; i++)
	{
		newSnake.segments[i*2] = currentSnake.segments[i*2 - 2], newSnake.segments[i*2 + 1] = currentSnake.segments[i*2 - 1];
		newSnake.segmentColors[i*3] = 0, newSnake.segmentColors[i*3 + 1] = 1,
		newSnake.segmentColors[i*3 + 2] = 0;
	}

	newSnake.segmentColors[0] = 0, newSnake.segmentColors[1] = 0,
	newSnake.segmentColors[2] = 1;
	newSnake.heading = currentSnake.heading;
	delete[] currentSnake.segments;
	delete[] currentSnake.segmentColors;
	return newSnake;
}

void blockBlink(void)
{
	if(goalBlink > 1.0) goalBlink = 0;
	goalColor[0] = goalBlink;
	goalColor[1] = goalBlink;
	goalColor[2] = goalBlink;
	goalBlink+=0.01;
}

void checkGoal(void)
{
	if(roger.segments[0] == goal[0] && roger.segments[1] == goal[1]) 
	{
		roger = addBodySegment(roger);
		randomNumberGenerator();
	}
}

//Step 4 prototype your display function
void display(void)
{
	//Set a clear color for the world, arguments are RGBA
	glClearColor(0,0,0,0);

	//Initialize the display mode, GL_DEPTH | GL_DOUBLE | GL_RGBA
	//This sets up the correct buffers needed for smooth drawing
	glutInitDisplayMode(GL_DEPTH | GL_DOUBLE | GL_RGBA);

	//Be sure to clear any unwanted information
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glLoadIdentity();

	//Step 5 Build a test to verify your set up
	//double color[] = {1,0,0};
	//int vertex[] = {0,0};

	//Step 6 setup the orthographic projection (i.e. the world grid)
	gluOrtho2D(0, WIDTH, 0, HEIGHT);
	glViewport(0,0,WIDTH, HEIGHT);

	glPointSize(4);

	glColorPointer(3, GL_FLOAT, 0, worldColors);
	glVertexPointer(2, GL_INT, 0, worldDraw);
	glDrawArrays(GL_POINTS, 0, 100*100);

	glColorPointer(3, GL_FLOAT, 0, goalColor);
	glVertexPointer(2, GL_INT, 0, goal);
	glDrawArrays(GL_POINTS, 0, 1);

	glColorPointer(3, GL_FLOAT, 0, roger.segmentColors);
	glVertexPointer(2, GL_INT, 0, roger.segments);
	glDrawArrays(GL_POINTS, 0, roger.numberOfSegments);

	glutPostRedisplay();
	glFlush();
}

void idle(void)
{
	blockBlink();
	updateSnakePosition(roger);
	checkGoal();
	_sleep(50);
}

//Step 3 add in main
void main(int argv, char * argc[])
{	
	initDrawArrays();
	setupSnake(roger);
	glutInit(&argv, argc);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Snake");
	//Step 4 prototype your display function

	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutMainLoop();
}

#endif