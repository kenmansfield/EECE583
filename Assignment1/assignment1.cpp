#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <unistd.h>
#include "graphics.h"
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;
using std::cout;


struct Point {
    int x;
    int y;
    Point() : x(0), y(0) {}
    Point(int x1, int y1) : x(x1), y(y1) {}
};

struct Wire {
    Point * Contacts;   //array of points
    int numContacts;
    Wire() : Contacts(0), numContacts(0) {}
};

struct NetFileStruct {
    int x;
    int y;
    int numCells;
    int numWires;
    Point * Cells;
    Wire * Wires;
    
    NetFileStruct() : x(0), y(0), numCells(0), numWires(0), Cells(0), Wires(0) {}
};

static void button_press (float x, float y);
static void drawscreen (void);
static void setRectFill(int x, int y, int color);
static void setTextFill(int x, int y, char * text, int color, int offset);
static bool TraceWire(Point output, int **circuit, std::vector<Point> &theNet);
static void Delay(int sleep);
static const int WIDTH = 1400;
static const int LEFT_EDGE = 5;
static const int TOP_EDGE = 20;
static int sBoxDim = WIDTH/2;

//These are modifiers based on the different modes selected by the user.
static float sDelayModifier = 1.0f;
static bool sNoAnimation = false;
static bool sfinalAnimation = true;
static string sFileName = "";
 
static NetFileStruct sNetfile;

int ** CreateBlankCircuit(int x, int y)
{
   int **newCircuit = new int*[x];
   for(int i = 0; i < x; i++)
   {
        newCircuit[i] = new int[y];
        for(int j = 0; j < y; j++)
        {
            newCircuit[i][j] = 0;
        }
   }
   return newCircuit;
}

void DeleteCircuit(int **circuit, int x)
{
	for (int i = 0; i < x; i++)
  	{
  		delete[] circuit[i];
  	}
	delete [] circuit;
}

void BuildCircuit(int **circuit, Point * obstructions, int size)
{
    //for each cell
    for(int i = 0; i < size; i++)
    {
        circuit[obstructions[i].x][obstructions[i].y] = -1;
        setRectFill(obstructions[i].x, obstructions[i].y, BLUE);
    }
    
    //for each input/output, mark as blocked.
    for(int i = 0; i < sNetfile.numWires; i++)
    {
        Wire theWire = sNetfile.Wires[i];
        for(int j = 0; j < theWire.numContacts; j++)
        {
            Point theContact = theWire.Contacts[j];
            circuit[theContact.x][theContact.y] = -1;
            //char buff[10];
            //sprintf(buff, "%i", i);
            //setTextFill(theContact.x, theContact.y, buff, GREEN, 22);
        }
    }
}

bool routeWire(Wire theWire, int **circuit, std::vector<Point> &theNet)
{
    int contactsFound = 0;
    
    //Buffer to hold expansionlist, could use a vector instead.
    //Point *expansionList = &theWire.Contacts[0];   //initialize this to the first contact.
    
    for(int i = 0; i < theWire.numContacts; i++)
    {
        //Need to make sure that the contacts for this wire are not blocked.
        circuit[theWire.Contacts[i].x][theWire.Contacts[i].y] = 0;
    }
    
    std::vector<Point> expansionList;
    expansionList.push_back(theWire.Contacts[0]);
    
    std::vector<Point> expansionListTemp;
    
    int gValue = 1;
    circuit[expansionList.at(0).x][expansionList.at(0).y] = gValue;
    setTextFill(expansionList.at(0).x, expansionList.at(0).y, "1", BLUE, 12);
    gValue++;
    
    //get all neighbours, up down left right only.
    while(expansionList.empty() == false && contactsFound < theWire.numContacts - 1)
    {
        for(int j = 0; j < expansionList.size(); j++)
        {
            Point current = expansionList.at(j);
            
            Point neighbours[4];
            neighbours[0] = Point(current.x, current.y + 1);
            neighbours[1] = Point(current.x, current.y - 1);
            neighbours[2] = Point(current.x - 1, current.y);
            neighbours[3] = Point(current.x + 1, current.y);
    
            for(int i = 0; i < 4; i++)
            {
                if(neighbours[i].x >= 0 && neighbours[i].x < sNetfile.x
                 && neighbours[i].y >= 0 && neighbours[i].y < sNetfile.y)
                {
                    //place obstruction at each one of these points.
                    if(circuit[neighbours[i].x][neighbours[i].y] == 0)
                    {
                        //Place our updated gValue here.   
                        circuit[neighbours[i].x][neighbours[i].y] = gValue;
                        
                        char buff[10];
                        sprintf(buff, "%i", gValue);                        
                        
                        //cout << "outputting: " << buff << " at " << neighbours[i].x << " - " << neighbours[i].y << endl;
                        
                        setTextFill(neighbours[i].x, neighbours[i].y, buff, BLUE, 12);
                        Delay(10);
                        
                        //Add these points to the expansion list.
                        expansionListTemp.push_back(neighbours[i]);
                        
                        //Now check if we caught up to our contact!
                        for(int j = 1; j < theWire.numContacts; j++)
                        {
                            if(neighbours[i].x == theWire.Contacts[j].x 
                                && neighbours[i].y == theWire.Contacts[j].y)    //change this to check all the contacts.
                            {
                                //need to break out of all our loops.
                                contactsFound++;
                            }
                        }
                    }
                }
            }
        }
        
        expansionList = expansionListTemp;
        expansionListTemp.clear();
        gValue++;
    }
    
    bool traceSuccess = true;
    if(contactsFound > 0)
    {
        //start at 1, since we only need the destination nodes.
        for(int i = 1; i < theWire.numContacts; i++)
        {
            if(TraceWire(theWire.Contacts[i], circuit, theNet) == false)
            {
                traceSuccess = false;
                break;
            }
        }
    }
    else
    {
        //cout << "failed to make connection\n";
        return false;
    }
    return traceSuccess;
}

static bool TraceWire(Point output, int **circuit, std::vector<Point> &theNet)
{
 //Now trace backward to place the net, and block off future nets from using
        //the same grid locations.
        //Look at all neighbours and pick the smallest number continue until we reach our start position.
        //cout << "contact found!\n";
        
        Point current = output;
        theNet.push_back(current);
        while(true)
        {   
            //cout << "1\n";
            Point neighbours[4];
            neighbours[0] = Point(current.x, current.y + 1);
            neighbours[1] = Point(current.x, current.y - 1);
            neighbours[2] = Point(current.x - 1, current.y);
            neighbours[3] = Point(current.x + 1, current.y);
            
            //pick the smallest neighbour.
            int smallestNumber = -1;
            Point smallestPoint(-1,-1);
            for(int i = 0; i < 4; i++)
            {
                if(neighbours[i].x >= 0 && neighbours[i].x < sNetfile.x
                    && neighbours[i].y >= 0 && neighbours[i].y < sNetfile.y)
                {
                    int gValue = circuit[neighbours[i].x][neighbours[i].y];
                    if( (smallestNumber >= gValue || smallestNumber <= 0) && gValue > 0)
                    {
                        smallestNumber = gValue;
                        smallestPoint = neighbours[i];
                    }
                }
            }
            
            if(smallestPoint.x == -1)
            {
               //failed to retrace, this shouldn't fail.
               //cout << "failed to retrace net\n";
               return false;
               break;
            }
            else
            {
               theNet.push_back(smallestPoint);
               current = smallestPoint;
               //cout << "smallestPoints : " << smallestPoint.x << " : " << smallestPoint.y << "   smallestValue: " << smallestNumber << endl;
               if(smallestNumber == 1)
               {
                    break;
               }
            }
        }
        return true;
}


int ParseNetfile()
{
    ifstream infile;
    infile.open(sFileName.c_str());
    int lineNumber = 0;
    
    std::string line;
    
    if(!infile.is_open())
    {
        return 0;
    }
    
    if( getline(infile, line) )
    {
        istringstream lineStream(line);
        lineStream >> sNetfile.x >> sNetfile.y;
    } else return 0;
    
    if( getline(infile, line) )
    {
        istringstream lineStream(line);
        lineStream >> sNetfile.numCells;
    } else return 0;
    
    sNetfile.Cells = new Point[sNetfile.numCells];
    
    for(int i = 0; i < sNetfile.numCells; i++)
    {
        if( getline(infile, line) )
        {
            istringstream lineStream(line);
            lineStream >> sNetfile.Cells[i].x >> sNetfile.Cells[i].y;
        } else return 0;        
    }
    
    if( getline(infile, line) )
    {
        istringstream lineStream(line);
        lineStream >> sNetfile.numWires;
    } else return 0;   
    
    sNetfile.Wires = new Wire[sNetfile.numWires];
    for(int i = 0; i < sNetfile.numWires; i++)
    {
        if( getline(infile, line) )
        {
            istringstream lineStream(line);
            lineStream >> sNetfile.Wires[i].numContacts;
            sNetfile.Wires[i].Contacts = new Point[sNetfile.Wires[i].numContacts];
            
            for(int j = 0; j < sNetfile.Wires[i].numContacts; j++)
            {
                lineStream >> sNetfile.Wires[i].Contacts[j].x >> sNetfile.Wires[i].Contacts[j].y;
            }
        } else return 0;        
    }
    //cout << "Netfile parsing complete\n";
}

static void drawgrid (void) {

/* redrawing routine for still pictures.  Graphics package calls  *
 * this routine to do redrawing after the user changes the window *
 * in any way.                                                    */
 
        if(sNoAnimation)
        {
             return;
        } 
 
// clearscreen();  /* Should be first line of all drawscreens */
 setfontsize (10);
 setlinestyle (SOLID);
 setlinewidth (2);
 setcolor (BLACK);
 
 sBoxDim = WIDTH/sNetfile.x;
 
 for(int i = 0; i < sNetfile.x; i++)
 {
    for(int j = 0; j < sNetfile.y; j++)
    {
        drawrect (i*sBoxDim, j*sBoxDim + TOP_EDGE, (i+1)*sBoxDim,( j+1)*sBoxDim + TOP_EDGE);
        flushinput();
    }
 }
 //setRectFill(0,0, BLUE);
}

static void setRectFill(int x, int y, int color)
{
    if(sNoAnimation)
    {
        return;
    }
    
    setlinewidth (5);
     setcolor (color);
    setlinestyle (SOLID);  
     
     fillrect (x*sBoxDim, y*sBoxDim + TOP_EDGE, (x+1)*sBoxDim, (y+1)*sBoxDim + TOP_EDGE);
     flushinput();
     
     //cout << "building: " << x << " : " << y << endl;
     //drawtext(i*sBoxDim + 10, j*sBoxDim + TOP_EDGE + 10, "x", 150); 
}

static void setTextFill(int x, int y, char * text, int color, int offset)
{
    if(sNoAnimation)
    {
        return;
    }
    setcolor(color);
    drawtext(x*sBoxDim + offset, y*sBoxDim + TOP_EDGE + offset, text, 150); 
    flushinput();
}

static void button_press (float x, float y) {

/* Called whenever event_loop gets a button press in the graphics *
 * area.  Allows the user to do whatever he/she wants with button *
 * clicks.                                                        */

 printf("User clicked a button at coordinates (%f, %f)\n", x, y);
}

void InitGraphics()
{
    /* initialize display */
     init_graphics("Lee-Moore Routing");

    /* still picture drawing allows user to zoom, etc. 
     init_world (0.,0.,1000.,1000.);
     update_message("Interactive graphics example.");
     event_loop(button_press, drawscreen);   */

    /* animation section */
     clearscreen();
     
     //event_loop(button_press, drawscreen);
     update_message("Non-interactive (animation) graphics example.");
     setcolor (RED);
     setlinewidth(0);
     setlinestyle (DASHED);
     init_world (0.,0.,WIDTH,WIDTH);
 
}

void PlaceWires(std::vector<Point> *nets, int **circuit)
{
    for(int i = 0; i < sNetfile.numWires; i++)
    {
        for(int j = 0; j < nets[i].size(); j++)
        {
            int color = (i) % 5 + 6;
            setRectFill(nets[i].at(j).x,nets[i].at(j).y, color);
            circuit[nets[i].at(j).x][nets[i].at(j).y] = -1;
        }
        
        Wire theWire = sNetfile.Wires[i];
        for(int j = 0; j < theWire.numContacts; j++)
        {
            Point theContact = theWire.Contacts[j];
            
            char buff[10];
            sprintf(buff, "%i", i);
            
            setTextFill(theContact.x, theContact.y, buff, BLACK, 12);
        }
    }
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

int main (int argc, char *argv[])
{
    //cout << "start\n";
    
    if(argc != 3)
    {
        cout << "bad arguments. usage = [assignment.exe filename mode] ie. [assignment.exe hello.net 1]\n Mode 0 = regular, mode 1 = faster animations, 2 = no animations, 3 = no display at all\n";
        return 0;
    }
    sFileName = string(argv[1]);
    
    int mode = atoi(argv[2]);
    //cout << "the mode: " << mode << endl;
    if(mode < 0 || mode > 3)
    {
        cout << "incorrect mode!\n";
        return 0;
    }
    else
    {
        if(mode == 0)
        {
            //do nothing.
        }
        else if(mode == 1)
        {
            sDelayModifier = 0.3f;
        }
        else if(mode == 2)
        {
            sDelayModifier = 0.f;
            sNoAnimation = true;        
        }
        else
        {
            sDelayModifier = 0.f;
            sNoAnimation = true;
            sfinalAnimation = false;
        }  
    }
  
    if(!ParseNetfile())
    {
        cout << "Parsing failed, exiting.\n";
        return 0;
    }
    
    if(mode != 3)
    {
        InitGraphics();
    }
    
    //Do routing for each wire.
    //Start by initializing a blank x by y circuit, then place each cell obstruction.
    //Pass a copy of this circuit to the route function to route the first Net.
    //Once the first net has been routed, place an obstruction at each point on the grid
    //and pass a copy to the 2nd circuit, etc. 
    
    std::vector<Point> nets[sNetfile.numWires];
    
    vector<int> prem;
    
    for(int i = 0; i < sNetfile.numWires; i++)
    {
        prem.push_back(i);
    }

    //We will try all premutations of ordering for placing the wires. Ie. wire 0,1,2 first, if that fails, then 0,2,1 etc.
    int iterations = 0; //gonna quit after a certain number of iterations so we don't have to wait too long.
    int numSuccesfullyWired = 0;
    int wireFailed = -1;
    bool success = false;
    

    
    do {
        numSuccesfullyWired = 0;
        for(int k = 0; k < sNetfile.numWires; k++)
        {
            nets[k].clear();
        }
        wireFailed = -1;
        
        for(int i = prem.size() - 1; i >= 0; i--)
        {
            int index = prem.at(i);
            drawgrid();
            int **theCurrentCircuit = CreateBlankCircuit(sNetfile.x, sNetfile.y);
            BuildCircuit(theCurrentCircuit, sNetfile.Cells, sNetfile.numCells);
            PlaceWires(nets, theCurrentCircuit);     
            
            success = routeWire(sNetfile.Wires[index], theCurrentCircuit, nets[index]);
            DeleteCircuit(theCurrentCircuit, sNetfile.x);
        
            Delay(800);
            if(sNoAnimation == false)
            {
                clearscreen();
            
            }
            
            if(success == false)
            {
                wireFailed = index;
            }
            else
            {
                numSuccesfullyWired++;
            }  
        }
        iterations++;
        if(iterations >= 10000)
        {
            //break;
        }
     
    } while( success == false && std::next_permutation(prem.begin(), prem.end()));
    
    bool overallSuccess = wireFailed == -1;      
    if(overallSuccess)
    {
        cout << "Attempted nets in this order: ";
        for (std::vector<int>::const_iterator p = prem.end() - 1; p != prem.begin() - 1; --p)
        {
            std::cout << *p << ' ';
        }
        cout << endl;
    
        cout << "*** Successfully routed all nets ***\n\n";
    }
    else
    {
        cout << "*** Unable to route this circuit after " << iterations << " premutations ***" 
        << "\nThis particular iteration failed on wire: " << wireFailed << "\nSuccessfully wired: " << numSuccesfullyWired << "\n\n";
    }
    
    if(mode == 2)
    {
        //Turn animation on to draw the final picture.
        sNoAnimation = false;
    }
    else if(mode == 3)
    {
        return 1;
    }
    
    drawgrid();
    int **theCurrentCircuit = CreateBlankCircuit(sNetfile.x, sNetfile.y);
    BuildCircuit(theCurrentCircuit, sNetfile.Cells, sNetfile.numCells);
    PlaceWires(nets, theCurrentCircuit);  

    while(true) {Delay(1000);}
}
