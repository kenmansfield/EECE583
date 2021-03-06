/*
 * Assignment2.hpp
 *
 *  Created on: Jan 31, 2016
 *      Author: km
 */

#ifndef ASSIGNMENT3_HPP_
#define ASSIGNMENT3_HPP_

#include <string>
#include <vector>
using namespace std;

struct Point {
	int x;
	int y;
	Point() :
			x(0), y(0) {
	}
	Point(int x1, int y1) :
			x(x1), y(y1) {
	}
};

struct Vertice {
	float x;
	float y;
	Vertice() :
			x(0), y(0) {
	}
	Vertice(float x1, float y1) :
			x(x1), y(y1) {
	}
};

struct Cell {
	vector<int> Nets;
	Point p;
};

bool ReadNetFile(std::string FileName);
bool ParseNetFile();
void InitGraphics();
void ColourRect(Point x);
Vertice GetLocationToDraw(Point P);
Point GetLocation(Point p);
void DrawGrid();
void setTextFill(Point p, string text, int color);
void DrawNumbers(Point p, int x);
void Delay(int time);
double DoAnnealing(double temperature, int iterations,
int numOfColumnsAndRowsToOmit, int &numAccepted);
void ScheduleAnneal();
double EstimateSTD();
void InitBoard();
float CostOfNet(int net);
float CostOfCell(int cellA);
float CostOfAllNets();
Point GetRandomPoint();
Point GetRandomPoint(Point p1, int windowShrinkage);
void Redraw();

//KL!
int mainKL(int argc, char *argv[]);
vector<float> calculateAllGains(vector<int> &vSides);
int ReturnSide(vector<int> &vSides, int cell);
bool DoesCrossBarrier(vector<int> &vSides, int a, int b);
int KLCostFunction(vector<int> &vSides);
void doSingleKL(vector<int> &vSides);
void DoKLSwap(vector<int> &vSides, int cellToSwap);
void doKL();
void InitKL();
void InitBoardKL(vector<int> &vSides);
void InitialPartition(vector<int> &vSides);
float calculateGainForCell(int iCell, vector<int> &vSides);
void updateGainsForConnectedNets(int cell, vector<float> &gains, vector<int> &vSides);
void updateGainForNet(int net, vector<float> &gains, vector<int> &vSides);
void DrawLines(vector<int> &vSides);

#endif /* ASSIGNMENT3_HPP_ */
