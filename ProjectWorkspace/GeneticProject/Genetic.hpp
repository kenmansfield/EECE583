/*
 * Project.hpp
 *
 *  Created on: Mar 18, 2016
 *      Author: km
 */

#ifndef PROJECT_HPP_
#define PROJECT_HPP_


int mainGenetic(int argc, char *argv[]);
void doGenetic();
int doSingleGenetic(vector<double> &vCurrentCosts, vector< vector<int> > &vAllChrom);
void doMutation(vector<int> &child);
vector<int> doCrossOver(vector< vector<int> > &vAllChrom, vector<int> &parent1, vector<int> &parent2);
void doReplace(vector< vector<int> > &vAllChrom, vector<int> &parents, vector<int> &child, vector<double> &vCurrentCosts, int childCost, int iWorst);
double hammingDistance(vector<int> &vect1, vector<int> &vect2);
double CostFunctionBUI(vector<double> &vCurrentCosts, vector<double> &vFit, int &iWorst, int &iBest);
double CostFunctionNAN(vector<double> &vCurrentCosts, vector<double> &vFit, vector< vector<int> > &vAllChrom);
int InitCosts(vector<double> &vCurrentCosts, vector< vector<int> > &vAllChrom);
int doGeneration(vector<double> &vCurrentCosts, vector< vector<int> > &vAllChrom);
void doGenerational(vector<double> &vCurrentCosts, vector< vector<int> > &vAllChrom);

#endif /* PROJECT_HPP_ */
