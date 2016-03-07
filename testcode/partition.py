
'''
initialize(blocklist, num_rows, num_cols):
to initialize a gird with random placement, create a grid first (using numpy) and fill it with -1. -1 will mean "empty".
Iterate all blocks:
    While a placement didn't happen yet:
        pick two random numbers (one for x, one for y)
        if(grid[x][y] is empty):
            place the block
            

TODO: remove blocklist            

'''

import numpy
import random
import sys
import math
import plotting
from os import listdir, path

class Partition:

    
    def __init__(self, filename):
        self.filename = filename
        self.netlist, self.blocklist, self.netsOfBlock, self.num_cells, self.num_rows, self.num_cols = self.readfile(filename)
        self.initialize()
    
    '''
    Initialization: create two lists: partA, partB. 
    To populate the partitions, first create a list of consecutive numbers, shuffle the list,
        and then take turns in populating partA and partB
    partA and Part are lists of block NUMBERS
    '''
    def initialize(self):
        self.gainOfBlock = numpy.zeros(len(self.blocklist), dtype=object)
        self.location = numpy.empty(len(self.blocklist), dtype=object)
        
        #The partitions are lists of block NUMBERS
        self.partA = []
        self.partB = []
        
        #The following partitions are SETS of UNLOCKED block NUMBERS
        self.partAUnlocked = set()
        self.partBUnlocked = set()
        
        #Populate sets
        self.partAUnlocked = set(self.partA)
        self.partBUnlocked = set(self.partB)
        
        #The following partitions are SETS of LOCKED block NUMBERS
        self.partALocked = set()
        self.partBLocked = set()

        
        
        
        self.initialPartition()
    
#         self.unlockAllBlocks();

#         print self.location

#         print self.gainOfBlock
#         print self.partA
#         print self.partAUnlocked
        return
    
    
#     '''
#     Random Initial partition
#     '''
#     def initialPartition(self):
#         turn = 0;
#         #Create list of consecutive numbers
#         blocknumbers = [i for i in xrange(self.num_cells)]
#         #Shuffle the list of consecutive numbers
#         random.shuffle(blocknumbers)
#         #Place the shuffled numbers in partA and partB, taking turns
#         for i in xrange(0, self.num_cells):
#             if(turn == 0):
#                 self.partA.append(blocknumbers[i])
#                 self.location[blocknumbers[i]] = 0
#                 turn = 1;
#             else:
#                 self.partB.append(blocknumbers[i])
#                 self.location[blocknumbers[i]] = 1
#                 turn = 0;
#         return
    
#     '''
#     Taking turns for Initial partition
#     '''
#     def initialPartition(self):
#         turn = 0;
#         #Create list of consecutive numbers
#         blocknumbers = [i for i in xrange(self.num_cells)]
#         #Shuffle the list of consecutive numbers
#         #random.shuffle(blocknumbers)
#         #Place the shuffled numbers in partA and partB, taking turns
#         for i in xrange(0, self.num_cells):
#             if(turn == 0):
#                 self.partA.append(blocknumbers[i])
#                 self.location[blocknumbers[i]] = 0
#                 turn = 1;
#             else:
#                 self.partB.append(blocknumbers[i])
#                 self.location[blocknumbers[i]] = 1
#                 turn = 0;
#         return
    
#     '''
#     Picking blocks with more connections first for Initial partition
#     '''
#     def initialPartition(self):
#         turn = 0;
#         blocklist_with_connections = []
#         for i in xrange(0, self.num_cells):
#             blocklist_with_connections.append((i, len(self.blocklist[i])))
#         blocklist_with_connections.sort(key= lambda r:r[1], reverse=True)
# #         print blocklist_with_connections
#         #Place the shuffled numbers in partA and partB, taking turns
#         for i in xrange(0, self.num_cells):
#             if(turn == 0):
#                 self.partA.append(blocklist_with_connections[i][0])
#                 self.location[blocklist_with_connections[i][0]] = 0
#                 turn = 1;
#             else:
#                 self.partB.append(blocklist_with_connections[i][0])
#                 self.location[blocklist_with_connections[i][0]] = 1
#                 turn = 0;
#         return
    '''
    Picking blocks, partA gets ones with most connections, partB gets ones with least connections
    '''
    def initialPartition(self):
        turn = 0;
        blocklist_with_connections = []
        for i in xrange(0, self.num_cells):
            blocklist_with_connections.append((i, len(self.blocklist[i])))
        blocklist_with_connections.sort(key= lambda r:r[1], reverse=True)
#         print blocklist_with_connections
        #Place the shuffled numbers in partA and partB, taking turns
        for i in xrange(0, self.num_cells/2):
            self.partA.append(blocklist_with_connections[i][0])
            self.location[blocklist_with_connections[i][0]] = 0
        for i in xrange(self.num_cells/2, self.num_cells):
            self.partB.append(blocklist_with_connections[i][0])
            self.location[blocklist_with_connections[i][0]] = 1
        return
            
    '''
    locate in which partition the blockIndex is. 0 for A, 1 for B
    '''
    def locate(self, block):
        return self.location[block]
    
    '''
    This function simply unlocks all blocks, by creating partAUnlocked & partBUnlocked from partA & partB
    '''
    def unlockAllBlocks(self):
        #Delete all elements in unlocked sets
        self.partAUnlocked.clear()
        self.partBUnlocked.clear()
        
        #Delete all elements in locked sets
        self.partALocked.clear()
        self.partBLocked.clear()
        
        #Populate sets
        for index in xrange(self.num_cells):
            if self.location[index] == 0:
                self.partAUnlocked.add(index)
            else:
                self.partBUnlocked.add(index)
        return    
    
    '''
    The cost function sums the cost of all nets (using costPerNet in a loop)
    Iterate through all NETS:
        calculate costPerNet, add it to totalCost.
    return totalCost
    This function goes through partA & partB. It doesn't capture the changes.
    To calculate cost after performing the partition passes, use finalTotalCost()
    '''
    def initialTotalCost(self):
        totalCost = 0
        for net in xrange(0, len(self.netlist)):
            cost_net = self.initialCostPerNet(self.netlist[net])
            totalCost += cost_net
#             self.costOfNet[net] = cost_net #add this later if incrementalCost is needed
#             print cost_net
        return totalCost
    
    def totalCost(self):
        totalCost = 0
        for net in xrange(0, len(self.netlist)):
            cost_net = self.costPerNet(self.netlist[net])
            totalCost += cost_net
#             self.costOfNet[net] = cost_net #add this later if incrementalCost is needed
#             print cost_net
        return totalCost    
    '''
    To calculate cost of specific NET:
    The inputs is the whole net, not just the index
    If all blocks related to this net is in one partition, cost of this net is zero. Otherwise, it's one.
    '''
    def initialCostPerNet(self, net):
        inPartA, inPartB = False, False
        for block in net:
            if self.locate(block) == 0:
                inPartA = True
            if self.locate(block) == 1:
                inPartB = True
            if(inPartA and inPartB):
                return 1
        return 0
    
    def costPerNet(self, net):
        inPartA, inPartB = False, False
        for block in net:
            if self.locate(block) == 0: #if block in partitionA
                inPartA = True
            if self.locate(block) == 1: #if block in partitionB
                inPartB = True
            if(inPartA and inPartB):
                return 1
        return 0
    
#     '''
#     The gainPerBlock function is defined as the improvement in cut-set size (totalCost) if this block is
#         swapped to the opposite partition.
#     The input is a block INDEX
#     Implementation: 
#                     locate whether the block is in partA or partB
#                     Iterate through relatedBlocks
#                         for each related block, find out whether they are in a DIFFERENT partition to the main block
#                             if different: gain = gain + 1
#     '''
#     def gainPerBlock(self, block):
#         #Locate where the block is (which partition)
#         inPartA, inPartB = False, False
#         if block in self.partALocked.union(self.partAUnlocked):
#             inPartA = True
#         if block in self.partBLocked.union(self.partBUnlocked):
#             inPartB = True
#             
# 
#         #Find whether relatedblocks are in same partition
#         gain = 0
#         for relatedBlock in self.blocklist[block]:
#             if (inPartA and relatedBlock in self.partBLocked.union(self.partBUnlocked)) or (inPartB and relatedBlock in self.partALocked.union(self.partAUnlocked)):
#                 gain = gain + 1
# #         print gain
#         return gain
    
    '''

    '''
    def gainPerBlock(self, block):
        #Locate where the block is (which partition)
        inPartA, inPartB = False, False
        if self.locate(block) == 0: #if block in partitionA
            inPartA = True
        if self.locate(block) == 1: #if block in partitionB
            inPartB = True
            

        #Find whether relatedblocks are in same partition
        gain = 0
        for relatedBlock in self.blocklist[block]:
            if (inPartA and self.locate(relatedBlock) == 1) or (inPartB and self.locate(relatedBlock) == 0):
                gain = gain + 3
            else:
                gain = gain - 1
#         print gain
#         print gain
        return gain
    
    '''
    To calculate total gain:
    Iterate through all blocks in blocklist:
        calculate gainPerBlock, add it to totalgain.
    return totalCost
    '''   
    def calcTotalGain(self):
        totalGain = 0
        for blockIndex in xrange(0, len(self.blocklist)):
            totalGain = totalGain + self.gainPerBlock(blockIndex)
#         print totalGain
        return totalGain
    
    
    '''
    Calculate gain of blocks:
    Iterate through all blocks in blocklist:
        calculate gainPerBlock, add it to gainOfBlock[blockIndex].
    '''   
    def calcGainOfBlocks(self):
        for blockIndex in xrange(0, len(self.blocklist)):
            self.gainOfBlock[blockIndex] = self.gainPerBlock(blockIndex)
        return 


    '''
    TODO
    input: blockIndex
    '''   
    def updateGainOfRelatedBlocks(self, block):
        relatedBlocks = self.blocklist[block]
        for blockIndex in relatedBlocks:
            self.gainOfBlock[blockIndex] = self.gainPerBlock(blockIndex)
        return
    
    
    '''
    This function returns the BLOCK INDEX of the highest gain of among unlocked blocks, in the input partition
    Implementation:
        Iterate through unlockedlocks in a specific partition. 
        Using block index, find block that corresponds to maximum gain (&store index), by using costOfBlock[index]
    '''
    def getBlockOfHighestGainOfUnlockedBlocks(self, partition):
        max = -sys.maxint - 1
        index = 0
        if partition == "A":
            for blockIndex in self.partAUnlocked:       #The values in partA & partB are just blocks' indices
                if max < self.gainOfBlock[blockIndex]:
                    max = self.gainOfBlock[blockIndex]
                    index = blockIndex
        if partition == "B":
            for blockIndex in self.partBUnlocked:
                if max < self.gainOfBlock[blockIndex]:
                    max = self.gainOfBlock[blockIndex]
                    index = blockIndex
#         print "gain: " + str(max)
        return index
    
    '''
    This function removes the input block from partAUnlocked or partBUnlocked SETS,
    and it moves the block to the other partition (in the LOCKED set)
    No need to specify whether the block is in partA or partB. The code will find it
    '''
    def moveBlockAndLock(self, blockIndex):
        if self.locate(blockIndex) == 0: #if block in partitionA
            self.partAUnlocked.remove(blockIndex)
            self.partBLocked.add(blockIndex)
            self.location[blockIndex] = 1;  #move to partitionB
        else:                            #if block in partitionB
            self.partBUnlocked.remove(blockIndex)
            self.partALocked.add(blockIndex)
            self.location[blockIndex] = 0;  #move to partitionA
        return
    
    '''
    Start with partition A, because it has either the same number of elements in B, or higher by 1
    randomize
    '''
    def partition(self):
        best_location = self.location
        plot = []
        pass_plot = []
        #save values of best iteration
        best_cost = sys.maxint
        number_of_passes = 0
        #do not do another pass if results are the same
        best_cost_per_pass = []
        for passes in xrange(0, 6):
         
            pass_plot = []    

#             print self.partA
#             print self.partB
            self.calcGainOfBlocks()
            self.unlockAllBlocks()
            
#             print len(self.partAUnlocked), len(self.partBUnlocked)
            if len(self.partAUnlocked) > len(self.partBUnlocked):
                turn = 0
            else:
                turn = 1;
                
            while(self.partAUnlocked or self.partBUnlocked):   #if either set is NOT empty
                if turn == 0 and self.partAUnlocked:
                    blockIndex = self.getBlockOfHighestGainOfUnlockedBlocks("A")
#                     print "index in A: " + str(blockIndex)
                    turn = 1
                    self.moveBlockAndLock(blockIndex)
                    self.updateGainOfRelatedBlocks(blockIndex)
                elif turn == 1 and self.partBUnlocked:
                    blockIndex = self.getBlockOfHighestGainOfUnlockedBlocks("B")
                    turn = 0
                    self.moveBlockAndLock(blockIndex)
                    self.updateGainOfRelatedBlocks(blockIndex)
                else:       #to exit last iteration
                    if turn == 1: turn = 0
                    elif turn == 0: turn = 1
#                     print "index in B: " + str(blockIndex)
                
#                 print "totalCost: " + str(self.totalCost())

                temp_cost = self.totalCost()
#                 print temp_cost
                if best_cost > temp_cost:
                    best_cost = temp_cost
                    best_location = numpy.copy(self.location)   #is this just a reference?
            
                pass_plot.append(temp_cost)
            plot.extend(pass_plot)
            self.location = numpy.copy(best_location)
#             print "Best Cost in Pass: " + str(best_cost)  
            best_cost_per_pass.append(best_cost)
            number_of_passes = number_of_passes + 1
            #do not do another pass if results are the same
            if(len(best_cost_per_pass)>=2):
                if best_cost_per_pass[len(best_cost_per_pass)-1] == best_cost_per_pass[len(best_cost_per_pass)-2]:
                    return best_cost, plot, number_of_passes
#             print len(self.partALocked), len(self.partBLocked)
            
        return best_cost, plot, number_of_passes
    '''
    Input File Format
    The circuit input format is as follows. The first line contains the number of cells to be placed, the number
    of connections between the cells, and the number of rows and columns upon which the circuit should be
    placed. Note that the product of the x and y dimensions should be at least as large as the number of cells in
    the circuit. In the following example, there are 3 cells, 4 connections between the cells, and the circuit
    should be placed on a chip with two rows of two cells each.
    The netlist file then contains one line per net. Each net can connect to two or more logic blocks. For each
    line, the first number is the number of logic blocks to which this net connects. The remaining numbers are
    the block numbers connected to this net. Note that blocks are numbered from 0. 
    
    From the file, we import netlists, but I find it better to represent the data as cell blocks, and each cell block contains the list of blocks it links to.
    (block) is (cell)
    blocklist = [block][list of other blocks it links to]
    How to do it:
    Iterate through all blocks (0 to num_cells):
        Iterate through all netlists:
            if cell exists in net:
                add all blocks in this net to the current block list (except if block_in_net == current_block???). Add to a SET Data Structure. Using Union function
                
    blocklist is a list of SETS
    '''
    def readfile(self, file):
        #Open the benchmark file
        f = open(file, 'r')
        
        #Read the first line: num_cells, num_connections, num_rows, num_cols
        num_cells, num_connections, num_rows, num_cols = [int(x) for x in f.readline().split()] # read first line
    #     print num_cells, num_connections, num_rows, num_cols;
        
        
        #Read the netlist.
        netlist = []
        for net in range(0, num_connections):
            temp = [int(y) for y in f.readline().split()]
            netlist.append(temp[1:])    #ignore the first element (python knows how many elements to read per line, by using the .split() function)
#         print netlist
    
        #Convert the netlist to blocklist
        blocklist = [set() for _ in xrange(num_cells)]
        for block in range (0, num_cells):
            for net in netlist:
                if block in net:
                    blocklist[block] |= set(net)
    #                 print block, net
         
        #Create netsOfBlock: set of all nets related to block
        netsOfBlock = [[] for _ in xrange(num_cells)]
        for block in range (0, num_cells):
            for index, net in enumerate(netlist):
                if block in net:
                    netsOfBlock[block].append(index)
                   
        return netlist, blocklist, netsOfBlock, num_cells, num_rows, num_cols

