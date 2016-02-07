/*
 * Assignment2.cpp
 *
 *  Created on: Jan 31, 2016
 *      Author: km
 */
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <unistd.h>
#include "graphics.h"
#include <string>
#include <sstream>
#include <algorithm>
#include "Assignment2.hpp"
#include <cassert>
#include <cmath>

using namespace std;

vector<Cell> sCells;
int ** sBoard = NULL;
vector< vector<int> > vNetFile;

float sBoxDim = 0;
static const int WIDTH = 1000;
float TOP_BORDER = 60;
float sWidthStretchForText = 1.5;

int sNumCells = 0;
int sNumConnection = 0;
int sNumRows = 0;
int sNumColumns = 0;

//These are modifiers based on the different modes selected by the user.
static float sDelayModifier = 1.0f;
static bool sNoAnimation = false;
static bool sfinalAnimation = true;

bool ReadNetFile(string fileName)
{
    ifstream infile;
    infile.open(fileName.c_str());
    int lineNumber = 0;

    std::string line;

    if(!infile.is_open())
    {
        return 0;
    }

    int i = 0;
    while( getline(infile, line) )
    {
    	vector<int> tempVec;
        istringstream lineStream(line);
        int x;
        while(lineStream >> x)
        {
        	tempVec.push_back(x);
        }
        vNetFile.push_back(tempVec);
        i++;
    }
//    cout << "hello world!!\n";
//
//    for(int i = 0; i < vNetFile.size(); i++)
//    {
//    	for(int j = 0; j < vNetFile.at(i).size(); j++)
//    	{
//    		cout << vNetFile.at(i).at(j) << " ";
//    	}
//
//    	cout << endl;
//
//    }
    return ParseNetFile();
}

bool ParseNetFile()
{
	sNumCells = vNetFile.at(0).at(0);
	sNumConnection = vNetFile.at(0).at(1);
	sNumRows = vNetFile.at(0).at(2);
	sNumColumns = vNetFile.at(0).at(3);

	//Removing the first line, so that our vector is now just nets.
	vNetFile.erase(vNetFile.begin());
	cout << "vnetfile size: " << vNetFile.size() << " num connections: " << sNumConnection << endl;

	//it seems like some files num connection doesn't match up to the actual number of nets!
	//assert(vNetFile.size() - 1 == sNumConnection);
}

void DrawGrid()
{
	int boxSize;

	setlinestyle (SOLID);
	setlinewidth (1);
	setcolor (BLACK);

	sBoxDim = WIDTH/(sNumRows * 2 + 1);

	for(int i = 0; i < sNumColumns; i++)
	{
		for(int j = 0; j < sNumRows; j++)
		{
			ColourRect(Point(i,j));
		}
		flushinput();
	}
	Point x;
}

void DrawNumbers(Point p, int x)
{
    char buff[10];
    sprintf(buff, "%i", x);
    string s(buff);
    setTextFill(p, s, BLACK);
    flushinput();
}

void setTextFill(Point p, string text, int color)
{
    if(sNoAnimation)
    {
        return;
    }

	//I think this is the top left, so need to offset it to the centre. And also make sure the font size is suitable.
	Vertice v = GetLocationToDraw(p);

	float font_height = sBoxDim/2;
	float offset = sWidthStretchForText*sBoxDim/2;
    setcolor(color);
    drawtext(v.x + offset, v.y + font_height, const_cast<char*>(text.c_str()), 100);
    //flushinput();
}

Point GetLocation(Point p)
{
	//There is one space between each row for the routing.
	int yOffset = (int(p.y - 1));
	int posY = (p.y) + yOffset;
	int posX = p.x;

	Point location(posX, posY);
	return location;
}

Vertice GetLocationToDraw(Point P)
{
	Point tempPoint = GetLocation(P);
	return Vertice(sBoxDim * sWidthStretchForText * float(tempPoint.x), sBoxDim * float(tempPoint.y)  + TOP_BORDER);
}

void ColourRect(Point x)
{
	setlinestyle (SOLID);
	setlinewidth (1);
	setcolor (BLACK);

	Vertice v = GetLocationToDraw(x);
	float offset = sBoxDim/2;
	drawrect (v.x, v.y, v.x + sBoxDim*sWidthStretchForText, v.y + sBoxDim);
}

static void button_press (float x, float y) {
}
static void drawscreen (void) {
	Redraw();
}

void InitGraphics()
{
	init_graphics("Simulated Annealing");

    //clearscreen();
    update_message("Simulated Annealing.");
    setcolor (RED);
    setlinewidth(1);
    init_world (0.,0.,WIDTH,WIDTH);
    //event_loop(button_press, drawscreen);
	setfontsize (8);
}

void Delay(int time)
{
    //convert from milliseconds to useconds
    if(sDelayModifier == 0)
    {
        return;
    }
    usleep(time * 1000 * sDelayModifier);
}

void InitBoard()
{
	//Initialize our empty board.
	sBoard = new int*[sNumColumns];
	for(int i = 0; i < sNumColumns; i++)
	{
		sBoard[i] = new int[sNumRows];
		for(int x = 0; x < sNumRows; x++)
		{
			sBoard[i][x] = -1;
		}
	}

	//Place each cell on the board and initialize our cells vector
	sCells.clear();
	sCells.resize(sNumCells);

	//cout << "\nwe have this many cells: " << sNumCells << "   thats it   -  ";
	for(int i = 0; i < sNumCells; i++)
	{
		int x = i % sNumColumns;
		int y = i / sNumColumns;
		sBoard[x][y] = i;

		//Initialize the current position of each cell
		sCells[i].p = Point(x,y);

		//And Draw!
		//cout << i << " ";
		DrawNumbers(Point(x,y), i);
	}

	for(int i = 0; i < vNetFile.size(); i++)
	{
		for(int j = 0; j < vNetFile[i].size(); j++)
		{
			int x = vNetFile[i][j];
			if (find(sCells[x].Nets.begin(), sCells[x].Nets.end(), i) == sCells[x].Nets.end())
			{
			  // only insert unique. just in case.
				sCells[x].Nets.push_back(i);
			}
		}
	}
}

void SwapCells(int cell1, int cell2, Point point1, Point point2)
{
	if(cell1 != -1)
	{
		sCells[cell1].p = point2;
	}
	if(cell2 != -1)
	{
		sCells[cell2].p = point1;
	}

	sBoard[point1.x][point1.y] = cell2;
	sBoard[point2.x][point2.y] = cell1;
}

void ScheduleAnneal()
{
	const int iterations = pow(sNumColumns*sNumRows, 4.f/3.f);
	int numAccepted = 0;

	//need to shake up the board a bit since I didn't randomize the board.
	//So do a pre-anneal with a small number of iterations.
	DoAnnealing(999999, iterations/5, 0, numAccepted);

	//Display starting cost.
	double startCost = CostOfAllNets();
	cout << "The start total is: " << startCost << endl;
	const int maxWindowShrinkage = float(min(sNumColumns, sNumRows))*.5;
	int currentWindowSizeReduction = 0;

	//start temperature.
	double STD = EstimateSTD();
	double Temperature = 20 * STD;

	int numberOfTemperatureIterations = 0;
	double acceptanceRate = 0;

	double fastAnnealB1 = 0.9;
	double fastAnnealB2 = 0.99;
	const bool doFastAnneal = false;

	double slowAnnealB1 = 0.99;
	double slowAnnealB2 = 0.999;

	while(true)
	{
		numAccepted = 0;
		DoAnnealing(Temperature, iterations, currentWindowSizeReduction, numAccepted);

		//Now calculate acceptance rate!
		acceptanceRate = ((double)numAccepted)/((double)(iterations));

		double Beta = slowAnnealB1;
		if(doFastAnneal)
		{
			Beta = fastAnnealB1;
			if(acceptanceRate < 0.5 && acceptanceRate > 0.25)
			{
				Beta = fastAnnealB2;
			}

		}
		else
		{
			if(acceptanceRate < 0.5 && acceptanceRate > 0.25)
			{
				Beta = slowAnnealB2;
			}
		}

		//New temperature
		Temperature = Beta*Temperature;

		//Determine if we should shrink the window!
		if(acceptanceRate < 0.44)
		{
			//decrease window size.
			if(currentWindowSizeReduction < maxWindowShrinkage)
			{
				currentWindowSizeReduction++;
			}
		}
		else if(acceptanceRate > 0.45)
		{
			if(currentWindowSizeReduction > 0)
			{
				currentWindowSizeReduction--;
			}
		}

		numberOfTemperatureIterations++;

		if(acceptanceRate <= 0.000001f)	//basically just want it to end when acceptance is 0.
		{
			//now, one last anneal at 0 temp!!
			DoAnnealing(0.f, iterations, currentWindowSizeReduction, numAccepted);
			DoAnnealing(0.f, iterations, currentWindowSizeReduction, numAccepted);
			break;
		}


		//Status update
		char buff[150];
		sprintf(buff, "T=%.4f AcptRt=%.5f TIterations=%i", Temperature, acceptanceRate,numberOfTemperatureIterations);
		update_message( buff );
		flushinput();
		Redraw();
		Delay(1);
	}

	char buff[150];
	sprintf(buff, "T=%.4f AcptRt=%.5f TIterations=%i", Temperature, acceptanceRate,numberOfTemperatureIterations);
	update_message( buff );
	Redraw();
	startCost = CostOfAllNets();
	cout << "The final total is: " << startCost << " after " << numberOfTemperatureIterations << " iterations" << endl;

}

double EstimateSTD()
{
	//pick 50 random nets and calculate STD.
	vector<int> costs;
	double mean = 0;
	for(int i = 0; i < 50; i++)
	{
		int randomNet = rand() % sNumConnection;
		int thisCost = CostOfNet(i);
		costs.push_back(thisCost);
		mean += thisCost;
	}

	if(costs.size() > 0)
	{
		mean = mean/costs.size();
		double sum = 0;
		for(int i = 0; i < costs.size(); i++)
		{
			double diff = (costs[i] - mean);
			sum += diff*diff;
		}
		return(sqrt(sum/costs.size()));
	}
	return 0;

}

double DoAnnealing(double temperature, int iterations, int numOfColumnsAndRowsToOmit, int &numAccepted)
{
	double STD = 0;
	double mean = 0;
	vector<double> deltas;
	for(int i = 0; i < iterations; i++)
	{
		//pick two random cells.
		Point p1 = GetRandomPoint();
		Point p2 = GetRandomPoint(p1, numOfColumnsAndRowsToOmit);

		int cell1 = sBoard[p1.x][p1.y];
		int cell2 = sBoard[p2.x][p2.y];

		double cost1 = CostOfCell(cell1) + CostOfCell(cell2);

		//now do swap
		SwapCells(cell1, cell2, p1, p2);

		double cost2 = CostOfCell(cell1) + CostOfCell(cell2);
		double costDelta = cost2 - cost1;

		bool accepted = true;
		//cost has gotten worse.
		if(cost2 >= cost1)
		{

			double sigma = 0;

			if(temperature > 0.001)
			{
				sigma = exp( -1 * costDelta / temperature );
			}

			double r = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
			//cout << "this is sigma: " << sigma << endl;
			if(sigma > r == false)
			{
				//go back.
				SwapCells(cell1, cell2, p2, p1);
				accepted = false;
			}
		}

		if(accepted)
		{
			numAccepted++;
		}
	}

	return STD;
}

Point GetRandomPoint()
{
	int r = rand();
	int randNumber = r % (sNumColumns * sNumRows);
	int x = randNumber % sNumColumns;
	int y = randNumber / sNumColumns;
	return Point(x,y);
}

Point GetRandomPoint(Point p1, int windowShrinkage)
{
	if(windowShrinkage == 0)
	{
		return GetRandomPoint();
	}
	//the columns/rows to include on either side of the point.
	int columns = sNumColumns - windowShrinkage;
	int rows = sNumRows - windowShrinkage;

	int y2 = min(p1.y + rows, sNumRows);
	int y1 = max(p1.y - rows, 0);
	int x2 = min(p1.x + columns, sNumColumns);
	int x1 = max(p1.x - columns, 0);

	int xDelta = x2 - x1;
	int yDelta = y2 - y1;

	int r = rand();
	int randNumber = r % (xDelta * yDelta);
	int x = x1 + (randNumber % xDelta);
	int y = y1 + (randNumber / xDelta);
	return Point(x,y);
}

float CostOfAllNets()
{
	int total = 0;
	for(int i = 0; i < vNetFile.size(); i++)
	{
		total += CostOfNet(i);
	}
	return total;
}

float CostOfNet(int net)
{
	//Bounding box means getting the biggest and smallest X and Y.
	int size = vNetFile[net].size();
	int minX = 0;
	int minY = 0;
	int maxX = 0;
	int maxY = 0;

	if(size > 0)
	{
		int x = vNetFile[net][0];
		Point loc = GetLocation(sCells[x].p);

		minX = loc.x;
		minY = loc.y;
		maxX = minX;
		maxY = minY;
	}

	for(int i = 1; i < vNetFile[net].size(); i++)
	{
		int x = vNetFile[net][i];
		Point loc = GetLocation(sCells[x].p);
		maxX = max(loc.x, maxX);
		maxY = max(loc.y, maxY);
		minX = min(loc.x, minX);
		minY = min(loc.y, minY);
	}

	float cost = abs(maxX - minX) + abs(maxY - minY);
	return cost;
}

float CostOfCell(int cellA)
{
	int netCost = 0;
	if(cellA >= 0 && cellA < sNumCells)
	{
		for(int i = 0; i < sCells[cellA].Nets.size(); i++)
		{
			netCost += CostOfNet(sCells[cellA].Nets[i]);
		}
	}
	return netCost;
}

void Redraw()
{
	clearscreen();
	DrawGrid();
	for(int i = 0; i < sCells.size(); i++)
	{
		DrawNumbers(sCells[i].p, i);
	}
}

int main (int argc, char *argv[])
{
	string FileName = "paira.txt";
	ReadNetFile(FileName);
	InitGraphics();

	DrawGrid();
	InitBoard();
	ScheduleAnneal();

    while(true) {Delay(1000);}
}

