
#include "StudentWorld.h"
#include <iostream>
using namespace std;

bool isCloseTo(const Actor* actor1, const Actor* actor2)
{
	return (actor1->getX() <= actor2->getX() + 30 && actor1->getX() >= actor2->getX() - 30)
		&& actor1->getY() >= actor2->getY();
}

bool overlap(const Actor* Actor1, const Actor* Actor2)
{
	double delta_x = abs(Actor1->getX() - Actor2->getX());
	double delta_y = abs(Actor1->getY() - Actor2->getY());
	double radiusSum = Actor1->getRadius() + Actor2->getRadius();

	if (delta_x < radiusSum * 0.25 && delta_y < radiusSum * 0.6)
		return true;

	return false;
}

bool isinBound(double x, double y)
{
	return (x >= 0 && x <= VIEW_WIDTH * 1.0) && (y >= 0 && y <= VIEW_HEIGHT * 1.0);
}

int twoChooseOne(int a, int b)
{
	int chance = randInt(0, 1);
	if (chance == 0)
		return a;
	else
		return b;
}

template<typename T>
void moveAndDelete(T* actor)
{
	double vert_speed = actor->getVertiSpeed() - actor->getWorld()->getPlayer()->getVertiSpeed();
	double horiz_speed = actor->getHorizSpeed();
	double new_y = actor->getY() + vert_speed;
	double new_x = actor->getX() + horiz_speed;
	actor->moveTo(new_x, new_y);

	if (!isinBound(new_x, new_y))
	{
		actor->setAliveStatus(false);
		return;
	}
}

template<typename T>
void pedestrainMove(T* actor)
{
	moveAndDelete(actor);

	if (actor->getMovePlan() > 0)
	{
		actor->setMovePlan(actor->getMovePlan() - 1);
		return;
	}

	else
	{
		actor->setHorizSpeed(twoChooseOne(randInt(-3, -1), randInt(1, 3)));
		actor->setMovePlan(randInt(4, 32));
		if (actor->getHorizSpeed() < 0)
			actor->setDirection(180);
		else
			actor->setDirection(0);
	}
}

void GhostRacer::doSomething()
{
	if (healthLevel() <= 0)
		setAliveStatus(false);
	if (!isActorAlive())
	{
		return;
	}
	double x = getX();
	if (x - 10 <= ROAD_CENTER - ROAD_WIDTH / 2.0)//left Border
	{
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
		if (getDirection() > 90)
			takeDamage(10);//hit the wall, take damage
		setDirection(82);//Change direction
	}

	if (x + 10 >= ROAD_CENTER + ROAD_WIDTH / 2.0)//right Border
	{
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
		if (getDirection() < 90)
			takeDamage(10);//hit the wall, take damage
		setDirection(98);//Change direction
	}

	//check input
	int ch;
	if (getWorld()->getKey(ch))
	{
		// user hits a key during this tick
		switch (ch)
		{
		case KEY_PRESS_RIGHT://turn right
			if (getDirection() > 66)
				setDirection(getDirection() - 8);
			break;
		case KEY_PRESS_LEFT://turn left
			if (getDirection() < 114)
				setDirection(getDirection() + 8);
			break;
		case KEY_PRESS_DOWN://slow down
			if (getVertiSpeed() > -1 )
				setVertiSpeed(getVertiSpeed() - 1);
			break;
		case KEY_PRESS_UP://accelerate
			if (getVertiSpeed() < 5)
				setVertiSpeed(getVertiSpeed() + 1);
			break;
		case KEY_PRESS_SPACE:
			if (getAmmo() > 0)
			{
				getWorld()->createProjectile(getWorld(), getX(), getY(), getDirection());
				getWorld()->playSound(SOUND_PLAYER_SPRAY);
				setAmmo(getAmmo() - 1);
			}
			break;
		}
	}


	//move the ghost racer according to direction
	double max_shift_per_tick = 4.0;
	double direction = getDirection();
	double delta_x = cos(direction * 3.14 / 180) * max_shift_per_tick;
	double cur_x = getX();
	double cur_y = getY();
	moveTo(delta_x + cur_x, cur_y);
}

void GhostRacer::spin()
{
	if (getDirection() - 20 > 60 && getDirection() + 20 < 120)
		setDirection(getDirection() + twoChooseOne(randInt(-20, -5), randInt(5, 20)));
	else if (getDirection() - 20 <= 60)
		setDirection(getDirection() + twoChooseOne(randInt(60-getDirection(), -5), randInt(5, 20)));
	else if (getDirection() + 20 > 120)
		setDirection(getDirection() + twoChooseOne(randInt(-20, -5), randInt(5, 120 - getDirection())));
}

void Border::doSomething()
{
	double vert_speed = getVertiSpeed() - getWorld()->getPlayer()->getVertiSpeed();
	double horiz_speed = getHorizSpeed();
	double new_y = getY() + vert_speed;
	double new_x = getX() + horiz_speed;
	moveTo(new_x, new_y);
	if (!isinBound(new_x, new_y))
	{
		setAliveStatus(false);
		return;
	}
}

WhiteBorder::WhiteBorder(double x, double y, StudentWorld* swp) :
	Border(IID_WHITE_BORDER_LINE, x, y,
		0, 2.0, 2, swp)
{
	getWorld()->setWhiteY(y);
}

void HumanPedestrain::doSomething()
{
	if (!isActorAlive())
		return;
	if (overlap(getWorld()->getPlayer(), this))
	{
		setAliveStatus(false);
		getWorld()->getPlayer()->setAliveStatus(false);
		return;
	}
	pedestrainMove(this);
}

void HumanPedestrain::getWet()
{
	setHorizSpeed(getHorizSpeed() * -1);
	if (getDirection() == 0)
		setDirection(180);
	else
		setDirection(0);
	getWorld()->playSound(SOUND_PED_HURT);
}

void Projectile::doSomething()
{
	if (!isActorAlive())
		return;

	getWorld()->overlapHolyWater(this);

	

	moveForward(SPRITE_HEIGHT);
	addToTraveled(SPRITE_HEIGHT);

	if (!isinBound(getX(), getY()))
	{
		setAliveStatus(false);
		return;
	}

	if(getTotalTraveled() >= getMax())
	{
		setAliveStatus(false);
		return;
	}
}

void ZombiePedestrain::getWet()
{
	takeDamage(5);
	if (!isActorAlive())
	{
		getWorld()->playSound(SOUND_PED_DIE);
		if (!overlap(getWorld()->getPlayer(), this))
		{
			int chance = randInt(0, 4);
			if (chance == 0)
				getWorld()->createHealGoodie(getWorld(), getX(), getY());
			getWorld()->increaseScore(150);
		}
	}
	else
		getWorld()->playSound(SOUND_PED_HURT);
}

void ZombiePedestrain::doSomething()
{
	if (!isActorAlive())
		return;

	if (overlap(this, getWorld()->getPlayer()))
	{
		takeDamage(10);
		getWorld()->playSound(SOUND_PED_DIE);
		getWorld()->getPlayer()->takeDamage(5);
		getWorld()->increaseScore(150);
	}

	if (isCloseTo(this, getWorld()->getPlayer()))
	{
		setDirection(270);

		if (getX() < getWorld()->getPlayer()->getX())//left
			setHorizSpeed(1);
		else if (getX() > getWorld()->getPlayer()->getX())//right
			setHorizSpeed(-1);

		setTicks(getTicks() - 1);//ready for next grunt

		if (getTicks() <= 0)
		{
			getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
			setTicks(20);
		}
	}

	pedestrainMove(this);
}

void HealthGoodie::doSomething()
{
	moveAndDelete(this);

	if (overlap(getWorld()->getPlayer(), this))
	{
		setAliveStatus(false);
		if (getWorld()->getPlayer()->healthLevel() <= 90)
			getWorld()->getPlayer()->takeDamage(-10);//take -10 damage = heal 10 health
		else
			getWorld()->getPlayer()->setHealth(100);

		getWorld()->playSound(SOUND_GOT_GOODIE);

		getWorld()->increaseScore(250);
	}
}

void HealthGoodie::getWet()
{
	setAliveStatus(false);//destroy the goodie
}

void OilSlick::doSomething()
{
	moveAndDelete(this);
	
	if (overlap(getWorld()->getPlayer(), this))
	{
		getWorld()->playSound(SOUND_OIL_SLICK);
		getWorld()->getPlayer()->spin();
	}
}

void WaterGoodie::doSomething()
{
	if (!isActorAlive())
		return;
	moveAndDelete(this);
	if (!isActorAlive())
		return;
	if (overlap(getWorld()->getPlayer(), this))
	{
		getWorld()->getPlayer()->setAmmo(getWorld()->getPlayer()->getAmmo() + 10);
		setAliveStatus(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(50);
	}
}

void LostSoul::doSomething()
{
	moveAndDelete(this);

	if (overlap(this, getWorld()->getPlayer()))
	{
		setAliveStatus(false);
		getWorld()->playSound(SOUND_GOT_SOUL);
		getWorld()->setSoul(getWorld()->getSoul()+1);
		getWorld()->increaseScore(100);
	}

	setDirection(getDirection() -10);

}

void ZombieCab::doSomething()
{
	if (!isActorAlive())
		return;

	//collision with player
	if (overlap(this, getWorld()->getPlayer()))
	{
		if (!getDamaged())
		{
			//playsound and get damage
			getWorld()->playSound(SOUND_VEHICLE_CRASH);
			getWorld()->getPlayer()->takeDamage(20);

			//left of player or same
			if (getX() <= getWorld()->getPlayer()->getX())
			{
				setHorizSpeed(-5);
				setDirection(120 + randInt(0, 19));
			}

			//left or same
			else if (getX() > getWorld()->getPlayer()->getX())
			{
				setHorizSpeed(5);
				setDirection(60 - randInt(0, 19));
			}
			setDamaged(true);//remember the hit
		}
	}

	//move and die
	moveAndDelete(this);
	if (!isActorAlive())
		return;

	//avoidance worthy object ahead when faster than player
	if (getVertiSpeed() >= getWorld()->getPlayer()->getVertiSpeed())
	{
		if(getWorld()-> sameLaneDetected(this) == 1)
		{
			setVertiSpeed(getVertiSpeed() - 0.5);
			return;
		}
	}

	//avoidance worthy object behind when slower than player
	if (getVertiSpeed() <= getWorld()->getPlayer()->getVertiSpeed())
	{
		if (getWorld()-> sameLaneDetected(this) == 2)
		{
			setVertiSpeed(getVertiSpeed() + 0.5);
			return;
		}
	}

	setMovePlan(getMovePlan() - 1);
	if (getMovePlan() > 0)
		return;

	setMovePlan(randInt(4, 32));
	setVertiSpeed(getVertiSpeed() + randInt(-2, 2));
}

void ZombieCab::getWet()
{
	takeDamage(5);
	if (!isActorAlive())
	{
		getWorld()->playSound(SOUND_VEHICLE_DIE);
		int chanceOil(randInt(1, 5));
		if (chanceOil == 1)
			getWorld()->createOilSlick(getWorld(), getX(), getY());
		getWorld()->increaseScore(200);
		return;
	}

	getWorld()->playSound(SOUND_VEHICLE_HURT);
}
