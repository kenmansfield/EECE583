/*
 * Assignment2.hpp
 *
 *  Created on: Jan 31, 2016
 *      Author: km
 */

#ifndef ASSIGNMENT2_HPP_
#define ASSIGNMENT2_HPP_

#include <string>
#include <vector>
using namespace std;

struct Point {
    int x;
    int y;
    Point() : x(0), y(0) {}
    Point(int x1, int y1) : x(x1), y(y1) {}
};

struct Vertice {
    float x;
    float y;
    Vertice() : x(0), y(0) {}
    Vertice(float x1, float y1) : x(x1), y(y1) {}
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
double DoAnnealing(double temperature, int iterations, int numOfColumnsAndRowsToOmit, int &numAccepted);
void ScheduleAnneal();
double EstimateSTD();
void InitBoard();
float CostOfNet(int net);
float CostOfCell(int cellA);
float CostOfAllNets();
Point GetRandomPoint();
Point GetRandomPoint(Point p1, int windowShrinkage);
void Redraw();

#endif /* ASSIGNMENT2_HPP_ */
