#include <array>
#include <chrono>
#include <SFML/Graphics.hpp>

#include "Headers/Global.hpp"
#include "Headers/Enemy.hpp"

Enemy::Enemy(unsigned char i_type, unsigned short i_x, unsigned short i_y) :
	direction(0 == (i_y / BASE_SIZE) % 2 ? -1 : 1),
	health(1 + i_type),
	hit_timer(0),
	type(i_type),
	x(i_x),
	y(i_y)
{
	
}

unsigned char Enemy::get_health() const
{
	return health;
}

unsigned char Enemy::get_hit_timer() const
{
	return hit_timer;
}

unsigned char Enemy::get_type() const
{
	return type;
}

unsigned short Enemy::get_x() const
{
	return x;
}

unsigned short Enemy::get_y() const
{
	return y;
}

void Enemy::hit()
{
	hit_timer = ENEMY_HIT_TIMER_DURATION;
}

void Enemy::move()
{
	if (0 != direction)
	{
		if ((1 == direction && x == SCREEN_WIDTH - 2 * BASE_SIZE) || (-1 == direction && x == BASE_SIZE))
		{
			//Once we reach the edge, we start moving down until we reach the next row.
			direction = 0;

			y += ENEMY_MOVE_SPEED;
		}
		else
		{
			//Moving horizontally.
			x = std::clamp<short>(x + ENEMY_MOVE_SPEED * direction, BASE_SIZE, SCREEN_WIDTH - 2 * BASE_SIZE);
		}
	}
	else
	{
		y = std::min<short>(y + ENEMY_MOVE_SPEED, BASE_SIZE * ceil(y / static_cast<float>(BASE_SIZE)));

		if (y == BASE_SIZE * ceil(y / static_cast<float>(BASE_SIZE)))
		{
			//Moving in a snake pattern. We use the modulo operator to decide whether to move left or right.
			direction = 0 == (y / BASE_SIZE) % 2 ? -1 : 1;
		}
	}
}

void Enemy::shoot(std::vector<Bullet>& i_enemy_bullets)
{
	switch (type)
	{
		case 0:
		{
			i_enemy_bullets.push_back(Bullet(0, ENEMY_BULLET_SPEED, x, y));

			break;
		}
		case 1:
		{
			i_enemy_bullets.push_back(Bullet(0.125f * ENEMY_BULLET_SPEED, ENEMY_BULLET_SPEED, x, y));
			i_enemy_bullets.push_back(Bullet(-0.125f * ENEMY_BULLET_SPEED, ENEMY_BULLET_SPEED, x, y));

			break;
		}
		case 2:
		{
			i_enemy_bullets.push_back(Bullet(0, ENEMY_BULLET_SPEED, x, y));
			i_enemy_bullets.push_back(Bullet(0.25f * ENEMY_BULLET_SPEED, ENEMY_BULLET_SPEED, x, y));
			i_enemy_bullets.push_back(Bullet(-0.25f * ENEMY_BULLET_SPEED, ENEMY_BULLET_SPEED, x, y));
		}
	}
}

void Enemy::update()
{
	if (0 < hit_timer)
	{
		if (1 == hit_timer)
		{
			health = std::max(0, health - 1);
		}

		hit_timer--;
	}
}

sf::IntRect Enemy::get_hitbox() const
{
	return sf::IntRect(x + 0.25f * BASE_SIZE, y + 0.25f * BASE_SIZE, 0.5f * BASE_SIZE, 0.5f * BASE_SIZE);
}