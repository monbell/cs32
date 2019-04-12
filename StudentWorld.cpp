#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

#include "Level.h"

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    levelIncrement = false;
    levelNum = 2; // CHANGE TO ZERO
    remainingCitizens = 0;
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

double StudentWorld::getPenelopeDistanceFrom(double x, double y)
{
    return(getDistFrom(x, y, playerP->getX(), playerP->getY()));
}

double StudentWorld::getDistFrom(double x, double y, double x2, double y2)
{
    double x_distSquared = (x - x2) * (x - x2);
    double y_distSquared = (y - y2) * (y - y2);
    return(x_distSquared + y_distSquared);
}


bool StudentWorld::penelopeInRange(double x, double y, double range)
{
    std::cout << x << " " << y << " " << playerP->getX() << " " << playerP->getY() << " , " << getDistFrom(x, y, playerP->getX(), playerP->getY()) << std::endl; // TAKE OUT
    
    return (getDistFrom(x, y, playerP->getX(), playerP->getY()) <= range*range);
}


bool StudentWorld::citizenInRange(double x, double y, double range)
{
    list<Actor*>::iterator it;
    it = objects.begin();
    
    while(it != objects.end())
    {
        if((*it)->isCitizen())
        {
            if(getDistFrom(x, y, (*it)->getX(), (*it)->getY()) <= range*range)
            {
                return true;
            }
        }
        it++;
    }
    return false;
}

double StudentWorld::getPenelopeX()
{
    return(playerP->getX());
}

double StudentWorld::getPenelopeY()
{
    return(playerP->getY());
}

int StudentWorld::init()
{
    levelNum++; // each time this function is called, the level is incremented
    Level lev(assetPath());
    
    string levelFile;
    switch(levelNum)
    {
        case 1:
        {
            levelFile = ("level01.txt");
            break;
        }
        case 2:
        {
            levelFile = ("level02.txt");
            break;
        }
        case 3:
        {
            levelFile = ("level03.txt");
            break;
        }
        case 4:
        {
            levelFile = ("level04.txt");
            break;
        }
        case 5:
        {
            levelFile = ("level05.txt");
            break;
        }
        case 6:
        {
            levelFile = ("level06.txt");
            break;
        }
        case 7:
            return GWSTATUS_PLAYER_WON;
        default:
        {
            levelFile = ("level01.txt");
            break;
        }
    }
    Level::LoadResult result = lev.loadLevel(levelFile);
    
    if (result == Level::load_success)
    {
        cerr << "Successfully loaded level" << endl;
        for(int x = 0; x < VIEW_WIDTH; x++)
        {
            for(int y = 0; y < VIEW_HEIGHT; y++)
            {
                Level::MazeEntry ge = lev.getContentsOf(x,y);
                switch (ge)
                {
                    case Level::empty:
                        break;
                    case Level::player:
                    {
                        playerP = new Penelope(SPRITE_WIDTH * x, SPRITE_HEIGHT * y, this);
                        break;
                    }
                    case Level::citizen:
                    {
                        remainingCitizens++;
                        Citizen* c = new Citizen(SPRITE_WIDTH * x, SPRITE_HEIGHT * y, this);
                        objects.push_back(c);
                        break;
                    }
                    case Level::dumb_zombie:
                    {
                        DumbZombie* z = new DumbZombie(SPRITE_WIDTH * x, SPRITE_HEIGHT * y, this);
                        objects.push_back(z);
                        break;
                    }
                    case Level::smart_zombie:
                    {
                        SmartZombie* z = new SmartZombie(SPRITE_WIDTH * x, SPRITE_HEIGHT * y, this);
                        objects.push_back(z);
                        break;
                    }
                    case Level::wall:
                    {
                        Wall* z = new Wall(SPRITE_WIDTH * x, SPRITE_HEIGHT * y, 0, this);
                        objects.push_back(z);
                        break;
                    }
                    case Level::exit:
                    {
                        Exit* z = new Exit(SPRITE_WIDTH * x, SPRITE_HEIGHT * y, 1, this);
                        objects.push_back(z);
                        break;
                    }
                    case Level::pit:
                    {
                        Pit* p = new Pit(SPRITE_WIDTH * x, SPRITE_HEIGHT * y, 0, this);
                        objects.push_back(p);
                        break;
                    }
                    case Level::vaccine_goodie:
                    {
                        VaccineGoodie* z = new VaccineGoodie(SPRITE_WIDTH * x, SPRITE_HEIGHT * y, this);
                        objects.push_back(z);
                        break;
                    }
                    case Level::gas_can_goodie:
                    {
                        GasCanGoodie* z = new GasCanGoodie(SPRITE_WIDTH * x, SPRITE_HEIGHT * y, this);
                        objects.push_back(z);
                        break;
                    }
                    case Level::landmine_goodie:
                    {
                        LandmineGoodie* z = new LandmineGoodie(SPRITE_WIDTH * x, SPRITE_HEIGHT * y, this);
                        objects.push_back(z);
                        break;
                    }
                    default:
                        break;
                }
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // stringstream
    std::ostringstream oss;
    oss.fill('0');
    oss << "Score: ";
    int s = getScore();
    oss << setw(6) << s << " Level: " << getLevel() << " Lives: " << getLives() << " Vacc: " << getVacc() << " Flames: " << getFlames() << " Mines: " << getMines() << " Infection: " << getInfection() << endl;
    setGameStatText(oss.str());
    
    if(playerP->getStatus()) // if alive
    {
        playerP->doSomething();
    }
    else
        return GWSTATUS_PLAYER_DIED;
    list<Actor*>::iterator it;
    it = objects.begin();
    while(it != objects.end())
    {
        if((*it)->getStatus()) // if alive
        {
            (*it)->doSomething();
            if(!(playerP->getStatus()))
            {
                decLives();
                if(getLives() == 0)
                    return GWSTATUS_PLAYER_DIED;
            }
        }
        it++;
    }
    
    it = objects.begin();
    while(it != objects.end())
    {
        if(!(*it)->getStatus()) // if dead
        {
            Actor* hold = *it; // deleting
            delete hold;
            it = objects.erase(it);
        }
        else
            it++;
    }
    
    if(levelIncrement == true)
    {
        levelIncrement = false;
        return GWSTATUS_FINISHED_LEVEL;
    }
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    if(playerP != nullptr)
    {
        delete playerP;
        playerP = nullptr;
    }
    delete playerP;
    
    // go through list and delete all objects
    list<Actor*>::iterator it;
    it = objects.begin();
    while(it != objects.end())
    {
        Actor* hold = *it;
        delete hold;
        it = objects.erase(it);
    }
}

bool StudentWorld::penelopeBlocked(int x, int y, Actor* obj) // returns true if an obj is blocked by penelope
{
    bool result = false;
    list<Actor*>::iterator it;
    it = objects.begin();
    
    while(it != objects.end())
    {
        if((*it)->getPenelopeBlocks())
        {
            if((*it)->getY() <= y+SPRITE_HEIGHT-1 && y <= ((*it)->getY()+SPRITE_HEIGHT-1) && (*it)->getX() <= (x+SPRITE_WIDTH-1) && ((*it)->getX()+SPRITE_WIDTH-1) >= x) // going up
            {
                result = true;
                break;
            }
            else if(((*it)->getY()+SPRITE_HEIGHT-1) >= y && y >= (*it)->getY() && (*it)->getX() <= (x+SPRITE_WIDTH-1) && ((*it)->getX()+SPRITE_WIDTH-1) >= x) // going down
            {
                result = true;
                break;
            }
            else if((*it)->getX() <= x+SPRITE_WIDTH-1 && x <= ((*it)->getX()+SPRITE_WIDTH-1) && (*it)->getY() <= (y+SPRITE_HEIGHT-1) && ((*it)->getY()+SPRITE_HEIGHT-1) >= y) // going left
            {
                result = true;
                break;
            }
            else if(((*it)->getX()+SPRITE_WIDTH-1) >= x && x >= (*it)->getX() && (*it)->getY() <= (y+SPRITE_HEIGHT-1) && ((*it)->getY()+SPRITE_HEIGHT-1) >= y) // going right
            {
                result = true;
                break;
            }
        }
        it++;
    }
    return result;
}

bool StudentWorld::zombieBlocked(int x, int y, Actor* obj) // returns true if there's any overlap b/w obj & zombie
{
    bool result = false;
    list<Actor*>::iterator it;
    it = objects.begin();
    
    while(it != objects.end())
    {
        if((*it) == obj) // if they're the same object
        {
            it++;
            if(it != objects.end())
            {
                if((*it)->getZombieBlocks()) // only checks the objects that zombie blocks
                {
                    if((*it)->getY() <= y+SPRITE_HEIGHT-1 && y <= ((*it)->getY()+SPRITE_HEIGHT-1) && (*it)->getX() <= (x+SPRITE_WIDTH-1) && ((*it)->getX()+SPRITE_WIDTH-1) >= x) // going up
                    {
                        result = true;
                        break;
                    }
                    else if(((*it)->getY()+SPRITE_HEIGHT-1) >= y && y >= (*it)->getY() && (*it)->getX() <= (x+SPRITE_WIDTH-1) && ((*it)->getX()+SPRITE_WIDTH-1) >= x) // going down
                    {
                        result = true;
                        break;
                    }
                    else if((*it)->getX() <= x+SPRITE_WIDTH-1 && x <= ((*it)->getX()+SPRITE_WIDTH-1) && (*it)->getY() <= (y+SPRITE_HEIGHT-1) && ((*it)->getY()+SPRITE_HEIGHT-1) >= y) // going left
                    {
                        result = true;
                        break;
                    }
                    else if(((*it)->getX()+SPRITE_WIDTH-1) >= x && x >= (*it)->getX() && (*it)->getY() <= (y+SPRITE_HEIGHT-1) && ((*it)->getY()+SPRITE_HEIGHT-1) >= y) // going right
                    {
                        result = true;
                        break;
                    }
                }
            }
        }
        else if((*it)->getZombieBlocks()) // only checks the objects that zombie blocks
        {
            if((*it)->getY() <= y+SPRITE_HEIGHT-1 && y <= ((*it)->getY()+SPRITE_HEIGHT-1) && (*it)->getX() <= (x+SPRITE_WIDTH-1) && ((*it)->getX()+SPRITE_WIDTH-1) >= x) // going up
            {
                result = true;
                break;
            }
            else if(((*it)->getY()+SPRITE_HEIGHT-1) >= y && y >= (*it)->getY() && (*it)->getX() <= (x+SPRITE_WIDTH-1) && ((*it)->getX()+SPRITE_WIDTH-1) >= x) // going down
            {
                result = true;
                break;
            }
            else if((*it)->getX() <= x+SPRITE_WIDTH-1 && x <= ((*it)->getX()+SPRITE_WIDTH-1) && (*it)->getY() <= (y+SPRITE_HEIGHT-1) && ((*it)->getY()+SPRITE_HEIGHT-1) >= y) // going left
            {
                result = true;
                break;
            }
            else if(((*it)->getX()+SPRITE_WIDTH-1) >= x && x >= (*it)->getX() && (*it)->getY() <= (y+SPRITE_HEIGHT-1) && ((*it)->getY()+SPRITE_HEIGHT-1) >= y) // going right
            {
                result = true;
                break;
            }
        }
        it++;
    }
    return result;
}

bool StudentWorld::citizenBlocked(int x, int y, Actor* obj) // for checkng objects blocked by citizens
{
    bool result = false;
    list<Actor*>::iterator it;
    it = objects.begin();
    
    while(it != objects.end())
    {
        if((*it) == obj) // if they're the same object
        {
            it++;
            if(it != objects.end())
            {
                if((*it)->getCitizenBlocks()) // only checks the objects that zombie blocks
                {
                    if((*it)->getY() <= y+SPRITE_HEIGHT-1 && y <= ((*it)->getY()+SPRITE_HEIGHT-1) && (*it)->getX() <= (x+SPRITE_WIDTH-1) && ((*it)->getX()+SPRITE_WIDTH-1) >= x) // going up
                    {
                        result = true;
                        break;
                    }
                    else if(((*it)->getY()+SPRITE_HEIGHT-1) >= y && y >= (*it)->getY() && (*it)->getX() <= (x+SPRITE_WIDTH-1) && ((*it)->getX()+SPRITE_WIDTH-1) >= x) // going down
                    {
                        result = true;
                        break;
                    }
                    else if((*it)->getX() <= x+SPRITE_WIDTH-1 && x <= ((*it)->getX()+SPRITE_WIDTH-1) && (*it)->getY() <= (y+SPRITE_HEIGHT-1) && ((*it)->getY()+SPRITE_HEIGHT-1) >= y) // going left
                    {
                        result = true;
                        break;
                    }
                    else if(((*it)->getX()+SPRITE_WIDTH-1) >= x && x >= (*it)->getX() && (*it)->getY() <= (y+SPRITE_HEIGHT-1) && ((*it)->getY()+SPRITE_HEIGHT-1) >= y) // going right
                    {
                        result = true;
                        break;
                    }
                }
            }
        }
        else if((*it)->getCitizenBlocks()) // only checks the objects that zombie blocks
        {
            if((*it)->getY() <= y+SPRITE_HEIGHT-1 && y <= ((*it)->getY()+SPRITE_HEIGHT-1) && (*it)->getX() <= (x+SPRITE_WIDTH-1) && ((*it)->getX()+SPRITE_WIDTH-1) >= x) // going up
            {
                result = true;
                break;
            }
            else if(((*it)->getY()+SPRITE_HEIGHT-1) >= y && y >= (*it)->getY() && (*it)->getX() <= (x+SPRITE_WIDTH-1) && ((*it)->getX()+SPRITE_WIDTH-1) >= x) // going down
            {
                result = true;
                break;
            }
            else if((*it)->getX() <= x+SPRITE_WIDTH-1 && x <= ((*it)->getX()+SPRITE_WIDTH-1) && (*it)->getY() <= (y+SPRITE_HEIGHT-1) && ((*it)->getY()+SPRITE_HEIGHT-1) >= y) // going left
            {
                result = true;
                break;
            }
            else if(((*it)->getX()+SPRITE_WIDTH-1) >= x && x >= (*it)->getX() && (*it)->getY() <= (y+SPRITE_HEIGHT-1) && ((*it)->getY()+SPRITE_HEIGHT-1) >= y) // going right
            {
                result = true;
                break;
            }
        }
        it++;
    }
    return result;
}

void StudentWorld::createVomit(double x, double y, Direction dir)
{
    Vomit* v = new Vomit(x, y, dir, this);
    objects.push_back(v);
}

void StudentWorld::createFlame(double x, double y)
{
    list<Actor*>::iterator it;
    it = objects.begin();
    
    while(it != objects.end())
    {
        if((*it)->isExit())
        {
            if(!((*it)->getX() == x && (*it)->getY() == y)) // if there can be a flame here
            {
                Flame* f = new Flame(x, y, this);
                objects.push_back(f);
            }
        }
        it++;
    }
}

void StudentWorld::createLandmine(double x, double y)
{
    Landmine* l = new Landmine(x, y, this);
    objects.push_back(l);
}

void StudentWorld::createPit(double x, double y)
{
    Pit* p = new Pit(x, y, 0, this);
    objects.push_back(p);
}

void StudentWorld::incrementPenelopesVaccines() { playerP->incrementVaccines(); }

void StudentWorld::incrementPenelopesLandmines() { playerP->incrementLandmines(); }

void StudentWorld::incrementPenelopesFlamethrowerCharges() { playerP->incrementFlamethrowerCharges(); }

int StudentWorld::getVacc() { return playerP->getVaccines(); }

int StudentWorld::getMines() { return playerP->getLandmines(); }

int StudentWorld::getFlames() { return playerP->getFlamethrowerCharges(); }

int StudentWorld::getInfection() { return playerP->getInfectionCount(); }

bool StudentWorld::penelopeOverlap(int x, int y) // returns true if there is an overlap with penelope
{
    bool result = false;
    if(playerP->getY() <= y+SPRITE_HEIGHT-1 && y <= (playerP->getY()+SPRITE_HEIGHT-1) && playerP->getX() <= (x+SPRITE_WIDTH-1) && (playerP->getX()+SPRITE_WIDTH-1) >= x) // going up
    {
        result = true;
    }
    else if((playerP->getY()+SPRITE_HEIGHT-1) >= y && y >= playerP->getY() && playerP->getX() <= (x+SPRITE_WIDTH-1) && (playerP->getX()+SPRITE_WIDTH-1) >= x) // going down
    {
        result = true;
    }
    else if(playerP->getX() <= x+SPRITE_WIDTH-1 && x <= (playerP->getX()+SPRITE_WIDTH-1) && playerP->getY() <= (y+SPRITE_HEIGHT-1) && (playerP->getY()+SPRITE_HEIGHT-1) >= y) // going left
    {
        result = true;
    }
    else if((playerP->getX()+SPRITE_WIDTH-1) >= x && x >= playerP->getX() && playerP->getY() <= (y+SPRITE_HEIGHT-1) && (playerP->getY()+SPRITE_HEIGHT-1) >= y) // going right
    {
        result = true;
    }
    return result;
}


bool StudentWorld::citizenOverlap(int x, int y) // returns true if there is an overlap with a citizen
{
    bool result = false;
    list<Actor*>::iterator it;
    it = objects.begin();
    
    while(it != objects.end())
    {
        if((*it)->isCitizen())
        {
            if((*it)->getY() <= y+SPRITE_HEIGHT-1 && y <= ((*it)->getY()+SPRITE_HEIGHT-1) && (*it)->getX() <= (x+SPRITE_WIDTH-1) && ((*it)->getX()+SPRITE_WIDTH-1) >= x) // going up
            {
                result = true;
            }
            else if(((*it)->getY()+SPRITE_HEIGHT-1) >= y && y >= (*it)->getY() && (*it)->getX() <= (x+SPRITE_WIDTH-1) && ((*it)->getX()+SPRITE_WIDTH-1) >= x) // going down
            {
                result = true;
            }
            else if((*it)->getX() <= x+SPRITE_WIDTH-1 && x <= ((*it)->getX()+SPRITE_WIDTH-1) && (*it)->getY() <= (y+SPRITE_HEIGHT-1) && ((*it)->getY()+SPRITE_HEIGHT-1) >= y) // going left
            {
                result = true;
            }
            else if(((*it)->getX()+SPRITE_WIDTH-1) >= x && x >= (*it)->getX() && (*it)->getY() <= (y+SPRITE_HEIGHT-1) && ((*it)->getY()+SPRITE_HEIGHT-1) >= y) // going right
            {
                result = true;
            }
        }
        it++;
    }
    return result;
}

bool StudentWorld::flameOrPitOverlap(int x, int y) // returns true if there is an overlap with a flame or a pit
{
    bool result = false;
    list<Actor*>::iterator it;
    it = objects.begin();
    
    while(it != objects.end())
    {
        if((*it)->isFlameOrPit())
        {
            if((*it)->getY() <= y+SPRITE_HEIGHT-1 && y <= ((*it)->getY()+SPRITE_HEIGHT-1) && (*it)->getX() <= (x+SPRITE_WIDTH-1) && ((*it)->getX()+SPRITE_WIDTH-1) >= x) // going up
            {
                result = true;
            }
            else if(((*it)->getY()+SPRITE_HEIGHT-1) >= y && y >= (*it)->getY() && (*it)->getX() <= (x+SPRITE_WIDTH-1) && ((*it)->getX()+SPRITE_WIDTH-1) >= x) // going down
            {
                result = true;
            }
            else if((*it)->getX() <= x+SPRITE_WIDTH-1 && x <= ((*it)->getX()+SPRITE_WIDTH-1) && (*it)->getY() <= (y+SPRITE_HEIGHT-1) && ((*it)->getY()+SPRITE_HEIGHT-1) >= y) // going left
            {
                result = true;
            }
            else if(((*it)->getX()+SPRITE_WIDTH-1) >= x && x >= (*it)->getX() && (*it)->getY() <= (y+SPRITE_HEIGHT-1) && ((*it)->getY()+SPRITE_HEIGHT-1) >= y) // going right
            {
                result = true;
            }
        }
        it++;
    }
    return result;
}

bool StudentWorld::infectableOverlap(int x, int y) // returns true if there is an overlap with an infectable object (penelope or citizen)
{
    bool result = false;
    list<Actor*>::iterator it;
    it = objects.begin();
    
    while(it != objects.end())
    {
        if((*it)->isInfectable())
        {
            if((*it)->getY() <= y+SPRITE_HEIGHT-1 && y <= ((*it)->getY()+SPRITE_HEIGHT-1) && (*it)->getX() <= (x+SPRITE_WIDTH-1) && ((*it)->getX()+SPRITE_WIDTH-1) >= x) // going up
            {
                result = true;
            }
            else if(((*it)->getY()+SPRITE_HEIGHT-1) >= y && y >= (*it)->getY() && (*it)->getX() <= (x+SPRITE_WIDTH-1) && ((*it)->getX()+SPRITE_WIDTH-1) >= x) // going down
            {
                result = true;
            }
            else if((*it)->getX() <= x+SPRITE_WIDTH-1 && x <= ((*it)->getX()+SPRITE_WIDTH-1) && (*it)->getY() <= (y+SPRITE_HEIGHT-1) && ((*it)->getY()+SPRITE_HEIGHT-1) >= y) // going left
            {
                result = true;
            }
            else if(((*it)->getX()+SPRITE_WIDTH-1) >= x && x >= (*it)->getX() && (*it)->getY() <= (y+SPRITE_HEIGHT-1) && ((*it)->getY()+SPRITE_HEIGHT-1) >= y) // going right
            {
                result = true;
            }
        }
        it++;
    }
    return result;
}

void StudentWorld::penelopeInfected() // sets penelope's infected status to true
{
    playerP->changeInfectionStatus();
    playerP->increment();
}

void StudentWorld::citizenInfected(int x, int y)
{
    return;
    /*
    // should change citizen's infection status to false;

     list<Actor*>::iterator it;
     it = objects.begin();
     
     while(it != objects.end())
     {
         if((*it)->isCitizen())
         {
             if((*it)->getX() == x && (*it)->getY() == y)
             {
             // (*it)->changeInfectionStatus();
             // (*it)->increment();
             }
             it++;
         }
     }
     */
}


// exit & level

int StudentWorld::getRemainingCitizens()
{
    return remainingCitizens;
}

void StudentWorld::decrementRemainingCitizens()
{
    remainingCitizens--;
}

void StudentWorld::levelUp()
{
    levelIncrement = true;
}
