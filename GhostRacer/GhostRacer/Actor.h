#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(const int imageID, double startX,
		double startY, int startDirection,
		double size, int depth, StudentWorld* swp, double vertiSpeed, double horizSpeed) :GraphObject(imageID, startX, startY,
			startDirection, size, depth)
	{
		setAliveStatus(true);
		setWorld(swp);
		setVertiSpeed(vertiSpeed);
		setHorizSpeed(horizSpeed);
	}


	double getVertiSpeed() { return m_vertiSpeed; }
	void setVertiSpeed(double newSpeed) { m_vertiSpeed = newSpeed; }

	double getHorizSpeed() { return m_horizSpeed; }
	void setHorizSpeed(double newSpeed) { m_horizSpeed = newSpeed; }

	virtual bool canGetWet() = 0;
	virtual void getWet() {}

	virtual bool avoidanceWorthy() = 0;

	void setAliveStatus(bool alive) { m_alive = alive; }
	virtual int getWhiteBorder() {return 255;}

	virtual void doSomething() = 0;
	bool isActorAlive() { return m_alive; }
	StudentWorld* getWorld() { return m_theWorld; }
protected:


	void setWorld(StudentWorld* swp) { m_theWorld = swp; }
private:
	StudentWorld* m_theWorld;
	bool m_alive;
	double m_vertiSpeed;
	double m_horizSpeed;
};

class Pedestrian : public Actor
{
public:
	Pedestrian(const int imageID, double startX,
		double startY, int startDirection,
		double size, int depth, int health, StudentWorld* swp, double vertiSpeed, double horizSpeed): 
		Actor(imageID, startX, startY,
			startDirection, size, depth, swp, vertiSpeed, horizSpeed) 
	{
		m_health = health;
	}

	int healthLevel()
	{
		return m_health;
	}	
	
	void takeDamage(int hurt)
	{
		m_health -= hurt;
		if (healthLevel() <= 0)
			setAliveStatus(false);
	}
	void setHealth(int newHealth)
	{
		m_health = newHealth;
	}



private:
	int m_health;
};

class Projectile : public Pedestrian
{
public:
	Projectile(StudentWorld* swp, double startX, double startY, int direction)
		:Pedestrian(IID_HOLY_WATER_PROJECTILE, startX, startY, direction, 1.0, 1, 10, swp, -4.0, 0)
	{
		setMax(160);
		m_totalTraveled = 0;
	}

	virtual void doSomething();
	virtual bool avoidanceWorthy() { return false; }

	virtual bool canGetWet() { return false; }

protected:
	double getMax() { return m_MaximumTravle; }
	void setMax(double newMax) { m_MaximumTravle = newMax; }

	void addToTraveled(double newTravel) { m_totalTraveled += newTravel; }
	double getTotalTraveled() { return m_totalTraveled; }

private:
	double m_MaximumTravle;
	double m_totalTraveled;
};

class ZombiePedestrain :public Pedestrian
{
public:
	ZombiePedestrain(StudentWorld* swp, double startX, double startY)
		:Pedestrian(IID_ZOMBIE_PED, startX, startY, 0, 3.0, 0, 10, swp, -4.0, 0)
	{
		setMovePlan(0);
		setTicks(0);
	}
	virtual bool canGetWet() { return true; }
	virtual void getWet();
	virtual bool avoidanceWorthy() { return true; }

	virtual void doSomething();

	void setMovePlan(double newPlan) { m_movePlan = newPlan; }
	double getMovePlan() { return m_movePlan; }
protected:
	void setTicks(double newTick) { m_ticksBeforeGrunt = newTick; }
	double getTicks() { return m_ticksBeforeGrunt; }

private:
	double m_ticksBeforeGrunt;
	double m_movePlan;
};

class HumanPedestrain : public Pedestrian
{
public:
	HumanPedestrain(StudentWorld* swp, double startX, double startY)
		:Pedestrian(IID_HUMAN_PED, startX, startY, 0, 2.0, 0, 10, swp, -4.0, 0)
	{
		setMovePlan(0);
	}
	virtual void doSomething();

	virtual bool canGetWet() { return true; }
	virtual void getWet();


	virtual bool avoidanceWorthy() { return true; }
	void setMovePlan(double newPlan) { m_movePlan = newPlan; }
	double getMovePlan() { return m_movePlan; }

private:
	double m_movePlan;
};

class GhostRacer : public Pedestrian
{
public:
	GhostRacer(StudentWorld* swp) :Pedestrian
		(IID_GHOST_RACER, 128, 32,90, 4.0, 0, 100, swp,0,0)
	{
		setAmmo(10);
	}
	int getAmmo() { return m_ammo; }

	virtual bool canGetWet() { return false; }


	virtual bool avoidanceWorthy() { return true; }
	virtual void doSomething();
	void spin();
	void setAmmo(int newAmmo) { m_ammo = newAmmo; }
private:
	int m_ammo;
};

class ZombieCab :public Pedestrian
{
public:
	ZombieCab(StudentWorld* swp, double startX, double startY, double vertiSpeed) :
		Pedestrian(IID_ZOMBIE_CAB, startX, startY, 90, 4.0, 0, 15, swp, vertiSpeed, 0)
	{
		setDamaged(false);
		setMovePlan(0);
	}

	virtual bool canGetWet() { return true; }
	virtual void getWet();

	virtual void doSomething();
	virtual bool avoidanceWorthy() { return true; }

	void setMovePlan(int moveplan) { m_moveplan = moveplan; }
	int getMovePlan() { return m_moveplan; }

protected:
	bool getDamaged() { return m_damaged; }
	void setDamaged(bool newStatus) { m_damaged = newStatus; }


private:
	bool m_damaged;
	int m_moveplan;

};

class Border :public Actor 
{
public:
	Border(const int imageID, double startX,
		double startY, int startDirection,
		double size, int depth, StudentWorld* swp) :
		Actor(imageID, startX, startY,
			startDirection, size, depth, swp, -4,0)
	{
		setAliveStatus(true);
	}
	virtual bool canGetWet() { return false; }
	virtual void doSomething();
	virtual bool avoidanceWorthy() { return false; }
private:
};

class WhiteBorder :public Border
{
public:
	WhiteBorder(double x, double y, StudentWorld* swp);

private:

};

class YellowBorder : public Border
{
public:
	YellowBorder(double x, double y, StudentWorld* swp) :
	Border(IID_YELLOW_BORDER_LINE, x, y,
		0, 2.0, 2, swp)
{
}

private:
};

class Goodie : public Actor
{
public:
	Goodie(const int imageID, double startX, double startY, StudentWorld* swp, int startDirection, int size)
		:Actor(imageID, startX, startY, startDirection, size, 2, swp, -4.0,0)
	{
	}
	virtual bool avoidanceWorthy() { return false; }
};

class HealthGoodie : public Goodie
{
public:
	HealthGoodie(double startX, double startY, StudentWorld* swp)
		:Goodie(IID_HEAL_GOODIE, startX, startY, swp, 0, 1) {}

	virtual void doSomething();


	virtual bool canGetWet() { return true; }
	virtual void getWet();
};

class OilSlick : public Goodie
{
public:
	OilSlick(StudentWorld* swp, double startX, double startY) :
		Goodie(IID_OIL_SLICK, startX, startY, swp, 0, randInt(2, 5))
	{

	}


	virtual void doSomething();
	virtual bool canGetWet() { return false; }
};

class WaterGoodie : public Goodie
{
public:
	WaterGoodie(StudentWorld* swp, double startX, double startY) :
		Goodie(IID_HOLY_WATER_GOODIE, startX, startY, swp, 90, 2)
	{

	}

	virtual void getWet() {setAliveStatus(false);}


	virtual bool canGetWet() { return true; }
	virtual void doSomething();
};

class LostSoul : public Goodie
{
public:
	LostSoul(StudentWorld* swp, double startX, double startY) :
		Goodie(IID_SOUL_GOODIE, startX, startY, swp, 0, 4)
	{

	}


	virtual bool canGetWet() { return false; }

	virtual void doSomething();

};

#endif // ACTOR_H_
