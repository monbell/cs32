#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(int imageID, double startX, double startY, Direction dir, int depth, StudentWorld* world);
    virtual ~Actor();
    virtual void doSomething() = 0;
    
    void changePenelopeBlocks(); // changes whether or not object blocks penelope
    bool getPenelopeBlocks(); // checks if an object is blocked by penelope
    void changeZombieBlocks(); // changes whether or not object blocks zombie
    bool getZombieBlocks(); // checks if an object is blocked by zombie
    void changeCitizenBlocks();
    bool getCitizenBlocks();
    
    StudentWorld* getWorld(); // gets student world
    
    virtual bool getStatus(); // gets if objects is alive or not
    virtual void changeStatus(); // changes whether an objects is alive or not
    
    void changeToCitizen(); // for doSomething() in zombie class & in landmine class
    bool isCitizen(); // for checking if citizen is in range in zombie class & checking if landmine overlaps w/ citizen (both through a StudentWorld class)
    
    void changeToExit();
    bool isExit();
    
    void changeToFlameOrPit();
    bool isFlameOrPit();
    
    void changeToInfectable();
    bool isInfectable();
    
private:
    bool penelopeBlocks;
    bool zombieBlocks;
    bool citizenBlocks;
    
    StudentWorld* myWorld;
    bool status;
    bool isAcitizen; // for doSomething() in zombie class
    bool isAnExit;
    bool isAFlameOrAPit; // works for penelope only right now
    bool isVomitInfectable;
};

class HasStatus : public Actor
{
public:
    HasStatus(int imageID, double startX, double startY, Direction dir, int depth, StudentWorld* world);
    virtual ~HasStatus();
    virtual void doSomething();
private:
};

class NoStatus : public Actor
{
public:
    NoStatus(int imageID, double startX, double startY, int depth, StudentWorld* world); // direction always right
    virtual ~NoStatus();
    virtual void doSomething();
private:
};

class FlameDamageable : public HasStatus
{
public:
    FlameDamageable(int imageID, double startX, double startY, int depth, StudentWorld* world);
    virtual ~FlameDamageable();
    virtual void doSomething();
    // function for checking if damaged
private:
    //
};

class Projectile : public HasStatus
{
public:
    Projectile(int imageID, double startX, double startY, Direction dir, StudentWorld* world);
    virtual ~Projectile();
    virtual void doSomething();
private:
    int tick;
};

class Vomit : public Projectile
{
public:
    Vomit(double startX, double startY, Direction dir, StudentWorld* world);
    virtual ~Vomit();
    virtual void doSomething();
private:
};

class Flame : public Projectile
{
public:
    Flame(double startX, double startY, StudentWorld* world);
    virtual ~Flame();
    virtual void doSomething();
private:
};

class Pit : public NoStatus
{
public:
    Pit(double startX, double startY, int depth, StudentWorld* world);
    virtual ~Pit();
    virtual void doSomething();
private:
};

class Wall : public NoStatus
{
public:
    Wall(double startX, double startY, int depth, StudentWorld* world);
    virtual ~Wall();
    virtual void doSomething();
private:
};

class Exit : public Actor
{
public:
    Exit(double startX, double startY, int depth, StudentWorld* world);
    virtual ~Exit();
    virtual void doSomething();
private:
};

class Moveable : public FlameDamageable
{
public:
    Moveable(int imageID, double startX, double startY, StudentWorld* world);
    virtual ~Moveable();
    virtual void doSomething();
private:
};

class Environment : public FlameDamageable
{
public:
    Environment(int imageID, double startX, double startY, int depth, StudentWorld* world);
    virtual ~Environment();
    virtual void doSomething();
};

class Goodie : public Environment
{
public:
    Goodie(int imageID, double startX, double startY, StudentWorld* world);
    virtual ~Goodie();
    virtual void doSomething();
private:
};

class VaccineGoodie : public Goodie
{
public:
    VaccineGoodie(double startX, double startY, StudentWorld* world);
    virtual ~VaccineGoodie();
    virtual void doSomething();
private:
};

class LandmineGoodie : public Goodie
{
public:
    LandmineGoodie(double startX, double startY, StudentWorld* world);
    virtual ~LandmineGoodie();
    virtual void doSomething();
private:
};

class GasCanGoodie : public Goodie
{
public:
    GasCanGoodie(double startX, double startY, StudentWorld* world);
    virtual ~GasCanGoodie();
    virtual void doSomething();
private:
};

class Landmine : public Environment
{
public:
    Landmine(double startX, double startY, StudentWorld* world);
    virtual ~Landmine();
    virtual void doSomething();
private:
    int safetyTics; // starts w/ 30
    bool state; // inactive
};

class Infectable : public Moveable
{
public:
    Infectable(int imageID, double startX, double startY, StudentWorld* world);
    virtual ~Infectable();
    virtual void doSomething();
    bool getInfectionStatus();
    bool getInfectionCount();
    void changeInfectionStatus();
    void increment(); // increment count & change status
private:
    bool infectionStatus;
    int infectionCount;
};

class Zombie : public Moveable
{
public:
    Zombie(double startX, double startY, StudentWorld* world);
    virtual ~Zombie();
    virtual void doSomething();
    void incrementDist();
    int getDist();
private:
    int movementPlanDistance;
    int tick;
};

class Penelope : public Infectable
{
public:
    Penelope(double startX, double startY, StudentWorld* world);
    virtual ~Penelope();
    virtual void doSomething();
    void incrementLandmines();
    void incrementFlamethrowerCharges();
    void incrementVaccines();
    int getLandmines();
    int getFlamethrowerCharges();
    int getVaccines();
    int getInfectionCount();
private:
    int landmines;
    int flamethrowerCharges;
    int vaccines;
};

class Citizen : public Infectable
{
public:
    Citizen(double startX, double startY, StudentWorld* world);
    virtual ~Citizen();
    virtual void doSomething();
private:
    int tick;
    double dist_p;
};

class DumbZombie : public Zombie
{
public:
    DumbZombie(double startX, double startY, StudentWorld * world);
    virtual ~DumbZombie();
    virtual void doSomething();
private:
};

class SmartZombie : public Zombie
{
public:
    SmartZombie(double startX, double startY, StudentWorld * world);
    virtual ~SmartZombie();
    virtual void doSomething();
private:
};

#endif // ACTOR_H_

