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
void doSingleGenetic(vector<double> &vCurrentCosts, vector< vector<int> > &vAllChrom);
void doMutation(vector<int> &child);
void doReplace(vector< vector<int> > &vAllChrom, vector<int> &parents, vector<int> &child, vector<double> &vCurrentCosts, int childCost, int iWorst);



#endif /* PROJECT_HPP_ */
