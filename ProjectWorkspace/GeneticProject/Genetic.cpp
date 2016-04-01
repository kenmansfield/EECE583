/*
 * Genetic.cpp
 *
 *  Created on: Mar 18, 2016
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
#include <cassert>
#include <cmath>
#include "main.hpp"
#include "Genetic.hpp"

extern int sNumCells;
extern string sFileName;
extern bool sfinalAnimation;
extern bool sDrawLines;

// Genetic Algorithm!
int mainGenetic(int argc, char *argv[])
{
	   if(argc < 2 || argc > 3)
	    {
	        cout << "bad arguments. usage: assignment3.exe [filename] [display mode]\nie. assignment3.exe hello.net\n\n";
	        cout << "Display mode 0 = normal, 1 = no lines, 2 = no display at all\n";
	        return 0;
	    }

	    sFileName = string(argv[1]);
	    int mode = 0;

	    if(argc == 3)
	    {
	    	mode = atoi(argv[2]);
	    }
	    //cout << "the mode: " << mode << endl;
	    if(mode < 0 || mode > 2)
	    {
	        cout << "incorrect mode!\n";
	        return 0;
	    }
	    else
	    {
	        if(mode == 0)
	        {
	        	sfinalAnimation = true;
	        	sDrawLines = true;
	        }
	        else if(mode == 1)
	        {
	        	sDrawLines = false;
	        }
	        else if(mode == 2)
	        {
	           	//do nothing.
		       	sDrawLines = false;
	        	sfinalAnimation = false;
	        }
	    }

	    // 0 - Read netfile
		if(!ReadNetFile(sFileName))
		{
			cout << "\n\n******Error reading netfile!!\n\n";
			return 0;
		}

		doGenetic();

	    // 2 - Once the KL is done, draw everything.
		return 1;
}

void doGenetic()
{
	// 1. generate 50 random (balanced) samples.
	vector< vector<int> > vAllChrom;
	vector<int> vSides;
	int half = sNumCells/2;
	for(int i = 0; i < sNumCells; i++)
	{
		if(i < half)
		{
			vSides.push_back(0);
		}
		else
		{
			vSides.push_back(1);
		}
	}

	InitKL();
	DrawGrid();
	InitBoardKL(vSides);

	vector<double> vCurrentCosts;

	for(int i = 0; i < 50; i++)
	{
		vector<int> vTemp = vSides;
		std::srand(i+1);
		random_shuffle(vTemp.begin(),vTemp.end());
		vTemp[0] = 0;

		if(i == 0)
		{
			for(int i = 0; i < 9; i++)
			{
				doSingleKL(vTemp);
			}
		}

		vAllChrom.push_back(vTemp);



		// 2. Calculate costs for all 50 chromosomes.
		double currentCut = KLCostFunction(vTemp);
		vCurrentCosts.push_back(currentCut);
	}

	for(int i = 0; i < 300000; i++)
	{
		doSingleGenetic(vCurrentCosts, vAllChrom);
	}

	if(sfinalAnimation)
	{
		clearscreen();
		InitBoardKL(vAllChrom[0]);
		DrawGrid();
		DrawLines(vAllChrom[0]);
		while(true) {Delay(1000);}
	}
}

void doSingleGenetic(vector<double> &vCurrentCosts, vector< vector<int> > &vAllChrom)
{
	// 3. Now calculate fitness function for each!
	// 1st have to find best and worst.
	double best = 999999;
	double worst = -999999;
	int iBest = 0;
	int iWorst = 0;

	for(int i = 0; i < vCurrentCosts.size(); i++)
	{
		int x = vCurrentCosts[i];
		if(x < best)
		{
			best = x;
			iBest = i;
		}
		if(x > worst)
		{
			worst = x;
			iWorst = i;
		}
	}

	vector<double> vFit;
	double totalFitness = 0;
	for(int i = 0; i < vCurrentCosts.size(); i++)
	{
		// Fi = (Cw - Ci) + (Cw - Cb)/3
		double Fi = double(worst - vCurrentCosts[i]) + (worst - best)/3.f;
		vFit.push_back(Fi);

		totalFitness += Fi;
		//cout << Fi << " ";
	}
	//cout << endl;

	// 4. Now select 2!
	// Probability of being selected is Fi/totalFitness - should add up to 1.

	vector<int> parents;

	int count = 0;
	while(parents.size() < 2)
	{
		double random = (((double) rand()) / (double) RAND_MAX)*totalFitness;
		double sum = 0;
		//Now pick the parent that corresponds to that fitness.
		for(int i = 0; i < vCurrentCosts.size(); i++)
		{
			sum += vFit[i];
			if(sum > random)
			{
				parents.push_back(i);
				//cout << "parent: " << vCurrentCosts[i] << "  " << vFit[i] << endl;
				break;
			}
		}
		if(count > 1000)
		{
			//return;
		}
		count++;
	}

	int half = sNumCells/2;
	// 4. Do the mating.
	// How to cut? For now, just divide in half.
	// Can do this by making a mask
	vector<bool> mask;

	//std::srand(i);


	for(int i = 0; i < sNumCells; i++)
	{
		mask.push_back(i < half);
	}
	//srand(time(NULL));
	//random_shuffle(mask.begin(),mask.end());

	vector<int> child1;
	vector<int> child2;
	for(int i = 0; i < sNumCells; i++)
	{
		if(mask[i])
		{
			int parent1 = parents[0];
			int x = vAllChrom[parent1][i];
			child1.push_back(x);
			child2.push_back(x);
		}
		else
		{
			int parent2 = parents[1];
			int x = vAllChrom[parent2][i];
			int xp = !vAllChrom[parent2][i];
			child1.push_back(x);
			child2.push_back(xp);
		}
	}

	// 5: do mutation to balance the child.
	doMutation(child1);
	doMutation(child2);

	vector<int> *child = &child1;

	int newCost = KLCostFunction(child1);
	int newCost2 = KLCostFunction(child2);
	if(newCost2 < newCost)
	{
		child = &child2;
		newCost = newCost2;
	}

	cout << "child cost: " << newCost << endl;
	doReplace(vAllChrom, parents, *child, vCurrentCosts, newCost, iWorst);

	//now do this until convergence?
}

void doReplace(vector< vector<int> > &vAllChrom, vector<int> &parents, vector<int> &child, vector<double> &vCurrentCosts, int childCost, int iWorst)
{
	// First, try to pick the worst parent to replace.
	bool parentChosen = false;
	int highest = childCost;
	int parent = -1;
	for(auto x : parents)
	{
		if(vCurrentCosts[x] > highest)
		{
			highest = vCurrentCosts[x];
			parent = x;
		}
	}

	if(parent >= 0)
	{
		vAllChrom[parent] = child;
		vCurrentCosts[parent] = childCost;
		return;
	}

	// child is worse than parents, so just replace the very worst in the population.
	vAllChrom[iWorst] = child;
	vCurrentCosts[iWorst] = childCost;
}

void doMutation(vector<int> &child)
{
	// Need to a certain minimum number of mutations.
	// add + 1 to make sure at least one mutation made.
	int mutations = ((rand() % sNumCells) / 100) + 0;
	int side0Count = 0;

	for(auto x : child)
	{
		if(x == 0)
		{
			side0Count++;
		}
	}

	int half = sNumCells/2;
	int diff = half - side0Count;

	mutations = max(mutations, abs(diff));
	while(mutations != 0)
	{
		int random  = rand() % sNumCells;

		if(diff < 0) // Then we need to add more to 0's.
		{
			if(child[random] == 0)
			{
				child[random] = 1;
				diff--;
				mutations--;
			}
		}
		else if(diff > 0)
		{
			if(child[random] == 1)
			{
				child[random] = 0;
				diff++;
				mutations--;
			}
		}
		else
		{
			// completely random.
			if(child[random] == 0)
			{
				child[random] = 1;
				diff--;
			}
			else
			{
				child[random] = 0;
				diff++;
			}
			mutations--;
		}
	}
}

