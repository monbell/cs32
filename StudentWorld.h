#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
#include "Actor.h"
#include <list>

// stringstream
#include <sstream>
#include <iomanip>
#include <string>

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool penelopeBlocked(int x, int y, Actor* obj);
    bool zombieBlocked(int x, int y, Actor* obj);
    bool citizenBlocked(int x, int y, Actor* obj);
    bool exitBlocked(int x, int y, Actor* obj);
    
    bool penelopeOverlap(int x, int y);
    bool citizenOverlap(int x, int y);
    bool flameOrPitOverlap(int x, int y);
    
    // vomit
    bool infectableOverlap(int x, int y);
    void penelopeInfected();
    void citizenInfected(int x, int y);
    
    void levelUp();
    
    double getDistFrom(double x, double y, double x2, double y2);
    double getPenelopeDistanceFrom(double x, double y);
    bool penelopeInRange(double x, double y, double range);
    bool citizenInRange(double x, double y, double range);
    
    double getPenelopeX();
    double getPenelopeY();
    
    // goodies
    void incrementPenelopesVaccines();
    void incrementPenelopesLandmines();
    void incrementPenelopesFlamethrowerCharges();
    int getVacc();
    int getMines();
    int getFlames();
    int getInfection();
    
    // projectiles
    void createVomit(double startX, double startY, Direction dir);
    void createFlame(double startX, double startY);
    void createLandmine(double startX, double startY);
    void createPit(double startX, double startY);
    
    // exit and level
    int getRemainingCitizens();
    void decrementRemainingCitizens();
    
private:
    //std::string display;
    Penelope* playerP;
    std::list<Actor*> objects; // only contains Wall objects for part 1
    int remainingCitizens;
    bool levelIncrement;
    int levelNum;
};

#endif // STUDENTWORLD_H_
