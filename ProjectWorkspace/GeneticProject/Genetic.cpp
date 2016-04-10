/*
 * Genetic.cpp
 *
 *  Created on: Mar 18, 2016
 *      Author: km
 */
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
#include <limits>
#include <random>
#include <iostream>
#include <cmath>

extern int sNumCells;
extern string sFileName;
extern bool sfinalAnimation;
extern bool sDrawLines;

const int POP_SIZE = 50;
bool isGenerational = false;
const int NUM_CROSSOVERS = 2;
bool DO_KL_ITERATION = false;

// Generational KL
// different cost function -> autobalances
// select parents
// add to mating pool
// pick random pair to mate
// random chance that it just passes through to next gen.
// mutate.
// start new gen.


// Genetic Algorithm!
int mainGenetic(int argc, char *argv[])
{
	   if(argc < 2 || argc > 5)
	    {
	        cout << "bad arguments. usage: assignment3.exe [filename] [display mode] [generational] [local improvement]\nie. assignment3.exe hello.net\n\n";
	        cout << "Display mode 0 = normal, 1 = no lines, 2 = no display at all\n";
	        return 0;
	    }

	    sFileName = string(argv[1]);
	    int mode = 0;

	    if(argc >= 3)
	    {
	    	mode = atoi(argv[2]);
	    }

	    if(argc >= 4)
	    {
		    // options.
	    	if(atoi(argv[3]) == 1)
	    	{
	    		isGenerational = true;
	    	}
	    }

	    if(argc >= 5)
	    {
		    // options.
	    	if(atoi(argv[4]) == 1)
	    	{
	    		DO_KL_ITERATION = true;
	    	}
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
	for(int i = 0; i < POP_SIZE; i++)
	{
		vector<int> vTemp = vSides;
		std::srand(i+1);
		random_shuffle(vTemp.begin(),vTemp.end());
		vTemp[0] = 0;
		vAllChrom.push_back(vTemp);
	}

	vector<int> side0Count;
	InitCosts(vCurrentCosts, vAllChrom);

	int lowestCost = std::numeric_limits<int>::max();
	int lowestCount = 0;

	if(isGenerational)
	{
		doGenerational(vCurrentCosts, vAllChrom);
	}
	else
	{
		int MAX_ITERATIONS = 600000;
		int MAX_REPETITIONS = 8000;

		if(DO_KL_ITERATION == true)
		{
			MAX_REPETITIONS /= 100;
			MAX_ITERATIONS /= 100;
		}

		for(int i = 0; i < MAX_ITERATIONS; i++)
		{
			int newCost = doSingleGenetic(vCurrentCosts, vAllChrom);
			if(newCost < lowestCost)
			{
				if(newCost == -1)
				{
					break;
				}
				lowestCost = newCost;
				lowestCount = 0;
			}
			else if(newCost == lowestCost)
			{
				lowestCount++;

				if(lowestCount == MAX_REPETITIONS)
				{
					break;
				}
			}
		}
		cout << "best: " << lowestCost << endl;
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

int InitCosts(vector<double> &vCurrentCosts, vector< vector<int> > &vAllChrom)
{
	vCurrentCosts.clear();
	const int COUNT = vAllChrom.size();
	int best = 9999999;
	for(int i = 0; i < COUNT; i++)
	{
		// 2. Calculate costs for all 50 chromosomes.
		double currentCut = KLCostFunction(vAllChrom[i]);
		vCurrentCosts.push_back(currentCut);
		if(currentCut < best)
		{
			best = currentCut;
		}
	}
	return best;
}

void doGenerational(vector<double> &vCurrentCosts, vector< vector<int> > &vAllChrom)
{
	int best = 99999999;
	int NUM_ITERATIONS = 60000;

	if(DO_KL_ITERATION)
	{
		NUM_ITERATIONS /= 100;
	}

	for(int i = 0; i < NUM_ITERATIONS; i++)
	{
		doGeneration(vCurrentCosts, vAllChrom);
		best = InitCosts(vCurrentCosts, vAllChrom);
	}
	cout << "Done, best cut: " << best << endl;
}

int doGeneration(vector<double> &vCurrentCosts, vector< vector<int> > &vAllChrom)
{
	vector<double> vFit;
	//double totalFitness = CostFunctionNAN(vCurrentCosts, vFit, vAllChrom);
	int iWorst;
	int iBest;
	double totalFitness = CostFunctionBUI(vCurrentCosts, vFit, iWorst, iBest);
	vector<int> matingPool;

	int matingPoolSize = vAllChrom.size()/2.f;

	vector<int> indices;
	for(int i = 0; i < POP_SIZE; i++)
	{
		indices.push_back(i);
	}

	int count = 0;
	int firstParent = -1;
	while(matingPool.size() < matingPoolSize)
	{
		int random = (((double) rand()) / (double) RAND_MAX)*totalFitness;
		double sum = 0;
		//Now pick the parent that corresponds to that fitness.
		for(int i = 0; i < vFit.size(); i++)
		{
			int prevSum = sum;
			sum += vFit[i];
			if(random >= prevSum && random <= sum)
			{
				//firstParent = i;
				matingPool.push_back(indices[i]);
				totalFitness -= vFit[i];
				vFit.erase(vFit.begin() + i);
				indices.erase(indices.begin() + i);
				break;
			}
		}
	}

	// 4: crossover!
	vector< vector <int>> newGen;
	newGen.push_back(vAllChrom[iBest]);

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, matingPoolSize - 1);

	while(newGen.size() < POP_SIZE)
	{
		int d1 = dis(gen);
		int d2 = dis(gen);
		int p1 = matingPool[d1];
		int p2 = matingPool[d2];
		if(p1 != p2)
		{
			vector<int> newChild = doCrossOver(vAllChrom, vAllChrom[p1], vAllChrom[p2]);
			doMutation(newChild);
			int newCost = KLCostFunction(newChild);
			int bestParent = vCurrentCosts[p1] < vCurrentCosts[p2] ? p1 : p2;
			int worstParent = vCurrentCosts[p1] > vCurrentCosts[p2] ? p1 : p2;

			double random = (((double) rand()) / (double) RAND_MAX);
			if(newCost <= vCurrentCosts[worstParent] || random < 0.3f)
			{
				if(DO_KL_ITERATION)
				{
					doSingleKL(newChild);
				}
				newGen.push_back(newChild);
			}
			else
			{

				newGen.push_back(vAllChrom[bestParent]);
			}
		}
	}
	vAllChrom = newGen;

	//now do this until convergence?
	return 1;
}

int doSingleGenetic(vector<double> &vCurrentCosts, vector< vector<int> > &vAllChrom)
{
	// 3. Now calculate fitness function for each!
	// 1st have to find best and worst.

	int iBest = 0;
	int iWorst = 0;

	vector<double> vFit;
	double totalFitness = CostFunctionBUI(vCurrentCosts, vFit, iWorst, iBest);

	// 4. Now select 2!
	// Probability of being selected is Fi/totalFitness - should add up to 1.

	vector<int> parents;
	int END_CONDITION = 20;
	int count = 0;
	while(parents.size() < 2)
	{
		double random = (((double) rand()) / (double) RAND_MAX)*totalFitness;
		double sum = 0;
		//Now pick the parent that corresponds to that fitness.
		for(int i = 0; i < vFit.size(); i++)
		{
			int prevSum = sum;
			sum += vFit[i];
			if(random >= prevSum && random <= sum)
			{
				if(parents.size() == 1)
				{
					if(parents[0] != i)
					{
						parents.push_back(i);
						break;
					}
				}
				else
				{
					parents.push_back(i);
					break;
				}
			}
		}
		if(count > END_CONDITION)
		{
			return -1;
		}
		count++;
	}
	int parent1 = parents[0];
	int parent2 = parents[1];

	// 4: crossover!
	vector<int> newChild = doCrossOver(vAllChrom, vAllChrom[parent1], vAllChrom[parent2]);

	// 5: do mutation to balance the child.
	doMutation(newChild);

	int newCost = KLCostFunction(newChild);
	//cout << "child cost: " << newCost << endl;

	// 6: Replacement!
	doReplace(vAllChrom, parents, newChild, vCurrentCosts, newCost, iWorst);

	//now do this until convergence?
	return newCost;
}

double CostFunctionBUI(vector<double> &vCurrentCosts, vector<double> &vFit, int &iWorst, int &iBest)
{
	double best = 999999;
	double worst = -999999;
	double totalFitness = 0;

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

	for(int i = 0; i < vCurrentCosts.size(); i++)
	{
		// Fi = (Cw - Ci) + (Cw - Cb)/3
		double Fi = double(worst - vCurrentCosts[i])*2.f + (worst - best)/9.f;
		vFit.push_back(Fi);

		totalFitness += Fi;
		//cout << Fi << " ";
	}
	return totalFitness;
}

double CostFunctionNAN(vector<double> &vCurrentCosts, vector<double> &vFit, vector< vector<int> > &vAllChrom)
{
	const int MAX = vCurrentCosts.size();
	double totalFitness = 0;
	const double r = 1.5;
	vFit.clear();
	for(int i = 0; i < MAX; i++)
	{
		//
		double v1 = 0;
		for(auto x : vAllChrom[i])
		{
			if(x == 0)
			{
				v1++;
			}
		}
		double v2 = vAllChrom[i].size() - v1;
		//

		double Fi = double(sNumCells - vCurrentCosts[i]); //*pow(1 - abs((v1 - v2)/(v1 + v2)), r);
		vFit.push_back(Fi);

		totalFitness += Fi;
	}
	return totalFitness;
}


void doReplace(vector< vector<int> > &vAllChrom, vector<int> &parents, vector<int> &child, vector<double> &vCurrentCosts, int childCost, int iWorst)
{
	if(DO_KL_ITERATION)
	{
		doSingleKL(child);
	}

	childCost = KLCostFunction(child);
	// Options:
	// 1: Replace worst parent, else replace worst in population.
	// 2: Replace closest parent (by hamming distance) if better, else replace worst in population?

	// First, try to pick the worst parent to replace.
	bool parentChosen = false;
	int highest = childCost;
	int parent = -1;
	for(auto x : parents)
	{
		if(vCurrentCosts[x] >= highest)
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
	//Options:
	//1: replace worst
	//2: only replace worst if child is better.
	//vAllChrom[iWorst] = child;
	//vCurrentCosts[iWorst] = childCost;

	//Tthis performs better (went from 302 to 300)
	if(vCurrentCosts[iWorst] >= childCost)
	{
		vAllChrom[iWorst] = child;
		vCurrentCosts[iWorst] = childCost;
	}
}

double hammingDistance(vector<int> &vect1, vector<int> &vect2)
{
	int disti = 0;
	for(int i = 0; i < vect1.size(); i++)
	{
		disti += (vect1[i] != vect2[i]);
	}
	double dist = 1.0*disti/vect1.size();
	return dist;
}

vector<int> doCrossOver(vector< vector<int> > &vAllChrom, vector<int> &parent1, vector<int> &parent2)
{
	// Add randomness to the crossover by choosing a different crossover point each time
	// randomly add +/- BIAS_SIZE % to choose the crossover point.
	const double BIAS_SIZE = .1f;
	//int half = 1 + (((double) rand()) / (double) RAND_MAX)*(sNumCells - 2);
	double bias = (((double) rand()) / (double) RAND_MAX)*BIAS_SIZE - BIAS_SIZE/2.f;
	//int half = sNumCells/2;
	//int half = sNumCells * (.5 + bias);
	// 4. Do the mating.
	// How to cut? For now, just divide in half.
	// Can do this by making a mask
	vector<bool> mask;
	mask.reserve(sNumCells);
	vector<int> crossoverPoints;
	double add = 1.f/(1 + NUM_CROSSOVERS);
	for(int i = 0; i < NUM_CROSSOVERS; i++)
	{
		double part = (add * (i + 1) + bias);
		crossoverPoints.push_back( part * sNumCells );
	}

	bool whichHalf = false;
	int iCross = 0;
	for(int i = 0; i < sNumCells; i++)
	{
		if(iCross < NUM_CROSSOVERS && i > crossoverPoints[iCross])
		{
			whichHalf = !whichHalf;
			iCross++;
		}
		mask.push_back(whichHalf);
	}

	vector<int> child1;
	vector<int> child2;
	child1.reserve(sNumCells);
	child2.reserve(sNumCells);
	int count1 = 0;
	int count2 = 0;
	for(int i = 0; i < sNumCells; i++)
	{
		if(mask[i])
		{
			int x = parent1[i];
			child1.push_back(x);
			child2.push_back(x);
			count1 += x;
			count2 += x;
		}
		else
		{
			int x = parent2[i];
			int xp = !parent2[i];
			child1.push_back(x);
			child2.push_back(xp);
			count1 += x;
			count2 += xp;
		}
	}

	// Now determine whether to return child1 or child2! Need to keep the normalized one
	// Bui and Moon just return the better child.
	// Some papers return the one with the minimum Hamming distance.
	// Other option is to return the least unbalanced one!

	// Return the one with the lowest cost;
	//int value1 = KLCostFunction(child1);
	//int value2 = KLCostFunction(child2);

	// Return the one with the closest hamming distance to parent 1;
	double value1 = hammingDistance(parent1, child1);
	double value2 = hammingDistance(parent1, child2);

	//double hamParents = hammingDistance(parent1, parent2);

	// Lets try returning the most balanced one, so that the mutation doesn't have to do too much rebalancing.
	//int value1 = abs(half - count1);
	//int value2 = abs(half - count1);

	if(value2 > value1)
	{
		return child1;
	}
	return child2;
}

// Mutation balances the child, and then
void doMutation(vector<int> &child)
{
	// Need to a certain minimum number of mutations.
	int mutations = std::round((std::rand() / (double)RAND_MAX)  * ( (sNumCells / 250) + 1));
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

	mutations = max(mutations, abs(diff) - 1);
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

