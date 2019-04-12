#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(int imageID, double startX, double startY, Direction dir, int depth, StudentWorld* world) : GraphObject(imageID, startX, startY, dir, depth, 1.0)
{
    status = true;
    myWorld = world;
    penelopeBlocks = false;
    isAcitizen = false;
    isAnExit = false;
    
    isAFlameOrAPit = false;
    isVomitInfectable = false;
    
    zombieBlocks = false;
}

Actor::~Actor() {}

bool Actor::getStatus() { return status; }

void Actor::changePenelopeBlocks() { penelopeBlocks = !penelopeBlocks; } // makes true for all objects that penelope blocks

void Actor::changeZombieBlocks() { zombieBlocks = !zombieBlocks; } // makes true for all objects that zombie blocks

bool Actor::getPenelopeBlocks() { return penelopeBlocks; }

bool Actor::getZombieBlocks() { return zombieBlocks; }

void Actor::changeCitizenBlocks() { citizenBlocks = !citizenBlocks; } // makes true for all objects that zombie blocks

bool Actor::getCitizenBlocks() { return citizenBlocks; }




void Actor::changeToCitizen() { isAcitizen = !isAcitizen; }

bool Actor::isCitizen() { return isAcitizen; }

void Actor::changeToExit() { isAnExit = !isAnExit; }

bool Actor::isExit() { return isAnExit; }

void Actor::changeToFlameOrPit() { isAFlameOrAPit = !isAFlameOrAPit; }

bool Actor::isFlameOrPit() { return isAFlameOrAPit; }

// vomit

void Actor::changeToInfectable() { isVomitInfectable = !isVomitInfectable; }

bool Actor::isInfectable() { return isVomitInfectable; }

//

StudentWorld* Actor::getWorld() { return myWorld; }

void Actor::changeStatus() { status = !status; }

HasStatus::HasStatus(int imageID, double startX, double startY, Direction dir, int depth, StudentWorld* world) : Actor(imageID, startX, startY, dir, depth, world) {}

HasStatus::~HasStatus() {}

void HasStatus::doSomething()
{
    if(!(getStatus())) // if player is dead
        return;
}

FlameDamageable::FlameDamageable(int imageID, double startX, double startY, int depth, StudentWorld* world) : HasStatus(imageID, startX, startY, right, depth, world) {}

FlameDamageable::~FlameDamageable() {}

void FlameDamageable::doSomething()
{
    HasStatus::doSomething();
}

Projectile::Projectile(int imageID, double startX, double startY, Direction dir, StudentWorld* world) : HasStatus(imageID, startX, startY, right, dir, world)
{
    tick = 0;
}

Projectile::~Projectile() {}

void Projectile::doSomething()
{
    HasStatus::doSomething();
    tick++;
    if(tick == 2) // shouldn't get to next tick
    {
        changeStatus();
    }
    // otherwise, damage all overlapping objects
}

Vomit::Vomit(double startX, double startY, Direction dir, StudentWorld* world) : Projectile(IID_VOMIT, startX, startY, dir, world) {}

Vomit::~Vomit() {}

void Vomit::doSomething()
{
    Projectile::doSomething();
    
    if(getWorld()->infectableOverlap(getX(), getY())) // if the vomit overlaps w/ Penelope or a citizen
    {
        if(getWorld()->penelopeOverlap(getX(), getY()))
        {
            getWorld()->penelopeInfected();
        }
        else
            getWorld()->citizenInfected(getX(), getY());
    }
}

Flame::Flame(double startX, double startY, StudentWorld* world) : Projectile(IID_FLAME, startX, startY, right, world) { changeToFlameOrPit(); }

Flame::~Flame() {}

void Flame::doSomething()
{
    Projectile::doSomething();
    // penelope does get damaged
}

Moveable::Moveable(int imageID, double startX, double startY, StudentWorld* world) : FlameDamageable(imageID, startX, startY, 0, world)
{
    changePenelopeBlocks();
    changeZombieBlocks();
} // zombie blocks all moveable objects

Moveable::~Moveable() {}

void Moveable::doSomething()
{
    FlameDamageable::doSomething();
}

Environment::Environment(int imageID, double startX, double startY, int depth, StudentWorld* world) : FlameDamageable(imageID, startX, startY, depth, world) {}

Environment::~Environment() {}

void Environment::doSomething()
{
    FlameDamageable::doSomething();
}

Infectable::Infectable(int imageID, double startX, double startY, StudentWorld* world) : Moveable(imageID, startX, startY, world)
{
    changeToInfectable();
    infectionStatus = false;
    infectionCount = 0;
}

Infectable::~Infectable() {}

void Infectable::doSomething()
{
    Moveable::doSomething();
    if(infectionCount == 500) //only works for Penelope
    {
        changeStatus();
        getWorld()->playSound(SOUND_PLAYER_DIE);
        // should create zombie here
        return;
    }
}

bool Infectable::getInfectionStatus() { return infectionStatus; }

bool Infectable::getInfectionCount() { return infectionCount; }

void Infectable::changeInfectionStatus() { infectionStatus = true; }

void Infectable::increment() { infectionCount++; }

Penelope::Penelope(double startX, double startY, StudentWorld* world) : Infectable(IID_PLAYER, startX, startY, world)
{
    landmines = 0;
    flamethrowerCharges = 0;
    vaccines = 0;
}

Penelope::~Penelope() {}

void Penelope::incrementLandmines() { landmines += 2; }

void Penelope::incrementFlamethrowerCharges() { flamethrowerCharges += 5; }

void Penelope::incrementVaccines() { vaccines++; }

int Penelope::getVaccines() { return vaccines; }

int Penelope::getLandmines() { return landmines; }

int Penelope::getInfectionCount() { return 0; }

int Penelope::getFlamethrowerCharges() { return flamethrowerCharges; }

void Penelope::doSomething()
{
    Moveable::doSomething();
    
    if(getWorld()->flameOrPitOverlap(getX(), getY()))
    {
        changeStatus();
        getWorld()->playSound(SOUND_PLAYER_DIE);
        return;
        //DETECT DEATH?
    }
    
    int ch;
    if(getWorld()->getKey(ch))
    {
        switch(ch)
        {
            case KEY_PRESS_SPACE:
            {
                if(flamethrowerCharges>0)
                {
                    flamethrowerCharges--;
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    for(int i = 0; i < 3; i++)
                    {
                        switch(getDirection())
                        {
                            case(up):
                                getWorld()->createFlame(getX(), getY()+i*SPRITE_HEIGHT);
                                break;
                            case(down):
                                getWorld()->createFlame(getX(), getY()-i*SPRITE_HEIGHT);
                                break;
                            case(left):
                                getWorld()->createFlame(getX()-i*SPRITE_WIDTH, getY());
                                break;
                            case(right):
                                getWorld()->createFlame(getX()+i*SPRITE_WIDTH, getY());
                                break;
                        }
                    }
                }
                break;
            }
            case KEY_PRESS_TAB:
            {
                if(landmines>0)
                {
                    landmines--;
                    getWorld()->createLandmine(getX(), getY());
                }
                break;
            }
            case KEY_PRESS_ENTER:
            {
                if(vaccines>0)
                {
                    vaccines--;
                    changeInfectionStatus();
                }
                break;
            }
            case KEY_PRESS_LEFT:
            {
                if(!(getWorld()->penelopeBlocked(getX()-4, getY(), this)))
                {
                    moveTo(getX()-4, getY());
                    setDirection(left);
                    break;
                }
                break;
            }
            case KEY_PRESS_RIGHT:
            {
                if(!(getWorld()->penelopeBlocked(getX()+4, getY(), this)))
                {
                    moveTo(getX()+4, getY());
                    setDirection(right);
                    break;
                }
                break;
            }
            case KEY_PRESS_UP:
            {
                if(!(getWorld()->penelopeBlocked(getX(), getY()+4, this)))
                {
                    moveTo(getX(), getY()+4);
                    setDirection(up);
                    break;
                }
                else
                    break;
            }
            case KEY_PRESS_DOWN:
            {
                if(!(getWorld()->penelopeBlocked(getX(), getY()-4, this)))
                {
                    moveTo(getX(), getY()-4);
                    setDirection(down);
                    break;
                }
                break;
            }
            default:
                break;
        }
    }
}

Citizen::Citizen(double startX, double startY, StudentWorld* world) : Infectable(IID_CITIZEN, startX, startY, world)
{
    
    changeToCitizen();
    dist_p = 0;
    tick = 0;
}

void Citizen::doSomething()
{
    tick++;
    if(tick%2 == 0) // paralysis tick
    {
        return;
    }
    
    dist_p = getWorld()->getPenelopeDistanceFrom(getX(), getY()); // distance between citizen and penelope
    
    
    if(getWorld()->penelopeInRange(getX(), getY(), 80))
    {
        if(getWorld()->getPenelopeY() == getY()) // same y value
        {
            if(getWorld()->getPenelopeX() < getX()) // if penelope is to the left of this citizen
            {
                if(!(getWorld()->penelopeOverlap(getX()-2, getY())) && !(getWorld()->citizenBlocked(getX()-2, getY(), this))) // can move 2 pixels left
                {
                    setDirection(left);
                    moveTo(getX()-2, getY());
                    return;
                }
            }
            else if(getWorld()->getPenelopeX() > getX()) // if penelope is to the right
            {
                if(!(getWorld()->penelopeOverlap(getX()+2, getY())) && !(getWorld()->citizenBlocked(getX()+2, getY(), this))) // can move 2 pixels right
                {
                    setDirection(right);
                    moveTo(getX()+2, getY());
                    return;
                }
            }
            // penelope must be to the left or right of the citizen since they can't be at the same place
            // runs below code if the citizen is blocked
            // skip to step 7
        }
        else if(getWorld()->getPenelopeX() == getX()) // same x value
        {
            if(getWorld()->getPenelopeY() < getY()) // if penelope is below this citizen
            {
                if(!(getWorld()->penelopeOverlap(getX(), getY()-2)) && !(getWorld()->citizenBlocked(getX(), getY()-2, this))) // can move 2 pixels left
                {
                    setDirection(down);
                    moveTo(getX(), getY()-2);
                    return;
                }
            }
            else if(getWorld()->getPenelopeY() > getY()) // if penelope is above
            {
                if(!(getWorld()->penelopeOverlap(getX(), getY()+2)) && !(getWorld()->citizenBlocked(getX(), getY()+2, this))) // can move 2 pixels right
                {
                    setDirection(up);
                    moveTo(getX(), getY()+2);
                    return;
                }
            }
            // runs below code if the citizen is blocked
            // skip to step 7
        }
        else // different x & y values
        {
            bool rand = randInt(0, 1);
            if(rand) // move in direction y
            {
                if(getWorld()->getPenelopeY() < getY()) // if penelope is below this citizen --> move up if possible
                {
                    if(!(getWorld()->penelopeOverlap(getX(), getY()-2)) && !(getWorld()->citizenBlocked(getX(), getY()-2, this))) // can move 2 pixels left
                    {
                        setDirection(down);
                        moveTo(getX(), getY()-2);
                        return;
                    }
                }
                else if(getWorld()->getPenelopeY() > getY()) // if penelope is above this citizen --> move up if possible
                {
                    if(!(getWorld()->penelopeOverlap(getX(), getY()+2)) && !(getWorld()->citizenBlocked(getX(), getY()+2, this))) // can move 2 pixels right
                    {
                        setDirection(up);
                        moveTo(getX(), getY()+2);
                        return;
                    }
                }
            }
            // code below runs if the random direction was not y OR if going in direction y blocked citizen
            if(getWorld()->getPenelopeX() < getX()) // penelope is to the left
            {
                if(!(getWorld()->penelopeOverlap(getX()-2, getY())) && !(getWorld()->citizenBlocked(getX()-2, getY(), this))) // can move 2 pixels left
                {
                    setDirection(left);
                    moveTo(getX()-2, getY());
                    return;
                }
            }
            else if(getWorld()->getPenelopeX() > getX()) // penelope is to the right
            {
                if(!(getWorld()->penelopeOverlap(getX()+2, getY())) && !(getWorld()->citizenBlocked(getX()+2, getY(), this))) // can move 2 pixels right
                {
                    setDirection(right);
                    moveTo(getX()+2, getY());
                    return;
                }
            }
        }
        return;
    }
    else
        return;
}

Citizen::~Citizen() {}

Zombie::Zombie(double startX, double startY, StudentWorld* world) : Moveable(IID_ZOMBIE, startX, startY, world)
{
    movementPlanDistance = 0;
    tick = 0;
}

void Zombie::doSomething()
{
    Moveable::doSomething();
    tick++;
    if(tick % 2 == 0) // paralysis tick
        return;
    
    int vomitX; // computing vomit coordinates
    int vomitY;
    
    if(getDirection() == left || getDirection() == right) // left or right direction
    {
        if(getDirection() == left) // left
        {
            vomitX = getX()-SPRITE_WIDTH;
            vomitY = getY();
        }
        else //right
        {
            vomitX = getX()+SPRITE_WIDTH;
            vomitY = getY();
        }
    }
    else // up or down direction
    {
        if(getDirection() == up) // up
        {
            vomitX = getX();
            vomitY = getY()+SPRITE_HEIGHT;
        }
        else //down
        {
            vomitX = getX();
            vomitY = getY()-SPRITE_HEIGHT;
        }
    }
    if(getWorld()->penelopeInRange(vomitX, vomitY, 10) || getWorld()->citizenInRange(vomitX, vomitY, 10))
    {
        int randChange = randInt(1, 3);
        if(randChange == 1)
        {
            getWorld()->createVomit(vomitX, vomitY, getDirection());
            getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
        }
    }
    
    if(movementPlanDistance == 0)
    {
        movementPlanDistance = randInt(3, 10); // generate random movement plan
        int dir = randInt(1, 4); // generate random direction
        switch(dir) // set new random direction
        {
            case 1:
            {
                setDirection(left);
                break;
            }
            case 2:
            {
                setDirection(right);
                break;
            }
            case 3:
            {
                setDirection(up);
                break;
            }
            case 4:
            {
                setDirection(down);
                break;
            }
            default:
                break;
        }
    }
    
    double newX = getX();
    double newY = getY();
    switch(getDirection()) // determine a destination coordinate that is 1 pixel in front of it in the direction it is facing
    {
        case left:
        {
            newX = getX() - 1;
            break;
        }
        case right:
        {
            newX = getX() + 1;
            break;
        }
        case up:
        {
            newY = getY() + 1;
            break;
        }
        case down:
        {
            newY = getY() - 1;
            break;
        }
        default:
            break;
    }
    
    if(!(getWorld()->zombieBlocked(newX, newY, this))) // if it can move in the direction without intersecting w/ wall, person or other zombie
    {
        moveTo(newX, newY); // then move
        movementPlanDistance--;
    }
    else
        movementPlanDistance = 0;
}

void Zombie::incrementDist() { movementPlanDistance++; }

int Zombie::getDist() { return movementPlanDistance; }

Zombie::~Zombie() {}

DumbZombie::DumbZombie(double startX, double startY, StudentWorld* world) : Zombie(startX, startY, world) {}

void DumbZombie::doSomething() { Zombie::doSomething(); }

DumbZombie::~DumbZombie() {}

SmartZombie::SmartZombie(double startX, double startY, StudentWorld* world) : Zombie(startX, startY, world) {}

void SmartZombie::doSomething() { Zombie::doSomething(); }

SmartZombie::~SmartZombie() {}

NoStatus::NoStatus(int imageID, double startX, double startY, int depth, StudentWorld* world) : Actor(imageID, startX, startY, right, depth, world) {}

NoStatus::~NoStatus() {}
void NoStatus::doSomething()
{
    if(!(getStatus()))
        return;
}

Goodie::Goodie(int imageID, double startX, double startY, StudentWorld* world) : Environment(imageID, startX, startY, 0, world) {}

Goodie::~Goodie() {}

void Goodie::doSomething()
{
    Environment::doSomething();
    
    if(getWorld()->penelopeOverlap(getX(), getY()))
    {
        getWorld()->increaseScore(50);
        changeStatus();
        getWorld()->playSound(SOUND_GOT_GOODIE);
    }
    
}

VaccineGoodie::VaccineGoodie(double startX, double startY, StudentWorld* world) : Goodie(IID_VACCINE_GOODIE, startX, startY, world) {}

VaccineGoodie::~VaccineGoodie() {}

void VaccineGoodie::doSomething()
{
    Goodie::doSomething();
    if(getWorld()->penelopeOverlap(getX(), getY()))
        getWorld()->incrementPenelopesVaccines();
}

LandmineGoodie::LandmineGoodie(double startX, double startY, StudentWorld* world) : Goodie(IID_LANDMINE_GOODIE, startX, startY, world) {}

LandmineGoodie::~LandmineGoodie() {}

void LandmineGoodie::doSomething()
{
    Goodie::doSomething();
    if(getWorld()->penelopeOverlap(getX(), getY()))
        getWorld()->incrementPenelopesLandmines();
}

GasCanGoodie::GasCanGoodie(double startX, double startY, StudentWorld* world) : Goodie(IID_GAS_CAN_GOODIE, startX, startY, world) {}

GasCanGoodie::~GasCanGoodie() {}

void GasCanGoodie::doSomething()
{
    Goodie::doSomething();
    if(getWorld()->penelopeOverlap(getX(), getY()))
        getWorld()->incrementPenelopesFlamethrowerCharges();
}


Landmine::Landmine(double startX, double startY, StudentWorld* world) : Environment(IID_LANDMINE, startX, startY, 1, world)
{
    safetyTics = 30;
    state = false;
}

Landmine::~Landmine() {}

void Landmine::doSomething()
{
    Environment::doSomething();
    if(!state) // if inactive
    {
        safetyTics--;
        if(safetyTics == 0)
            state = true; // becomes active if 0 safety ticks left
        return;
    }
    if(getWorld()->penelopeOverlap(getX(), getY()) || getWorld()->citizenOverlap(getX(), getY()))
    {
        state = false; // will be removed from the game by the StudentWorld object at the end of the current tick
        getWorld()->playSound(SOUND_LANDMINE_EXPLODE);
        
        getWorld()->createFlame(getX(), getY());
        getWorld()->createFlame(getX(), getY()+SPRITE_HEIGHT); // north
        getWorld()->createFlame(getX()+SPRITE_WIDTH, getY()+SPRITE_HEIGHT); // northeast
        getWorld()->createFlame(getX()+SPRITE_WIDTH, getY()); // east
        getWorld()->createFlame(getX()+SPRITE_WIDTH, getY()-SPRITE_HEIGHT); // southeast
        getWorld()->createFlame(getX(), getY()-SPRITE_HEIGHT); // south
        getWorld()->createFlame(getX()-SPRITE_WIDTH, getY()-SPRITE_HEIGHT); // southwest
        getWorld()->createFlame(getX()-SPRITE_WIDTH, getY()-SPRITE_HEIGHT); // southwest
        getWorld()->createFlame(getX()-SPRITE_WIDTH, getY()); // west
        getWorld()->createFlame(getX()-SPRITE_WIDTH, getY()+SPRITE_HEIGHT); // northwest
        getWorld()->createPit(getX(), getY());
    }
}

Pit::Pit(double startX, double startY, int depth, StudentWorld* world) : NoStatus(IID_PIT, startX, startY, 0, world) { changeToFlameOrPit(); }

Pit::~Pit() {}

void Pit::doSomething() { NoStatus::doSomething(); }

Wall::Wall(double startX, double startY, int depth, StudentWorld* world) : NoStatus(IID_WALL, startX, startY, depth, world)
{
    changePenelopeBlocks();
    changeZombieBlocks();
}

Wall::~Wall() {}

void Wall::doSomething() { NoStatus::doSomething(); }

Exit::Exit(double startX, double startY, int depth, StudentWorld* world) : Actor(IID_EXIT, startX, startY, right, depth, world) { changeToExit(); }

Exit::~Exit() {}

void Exit::doSomething()
{
    if(getWorld()->citizenOverlap(getX(), getY()))
    {
        std::cout << getWorld()->getRemainingCitizens() << std::endl;
        if(getWorld()->getRemainingCitizens()==0) // if there are no more citizens
        {
            getWorld()->increaseScore(500);
            if(getWorld()->penelopeOverlap(getX(), getY())) // increment level if penelope led the citizen to the exit (basically true in all cases)
            {
                getWorld()->levelUp();
            }
        }
        else
        {
            getWorld()->increaseScore(500);
            getWorld()->decrementRemainingCitizens();
            // set citizen state to dead
            getWorld()->playSound(SOUND_CITIZEN_SAVED);
        }
    }
    return;
}
