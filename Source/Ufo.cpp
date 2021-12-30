#include <array>
#include <chrono>
#include <random>
#include <SFML/Graphics.hpp>

#include "Headers/Animation.hpp"
#include "Headers/Global.hpp"
#include "Headers/Ufo.hpp"

Ufo::Ufo(std::mt19937_64& i_random_engine) :
	y(BASE_SIZE),
	powerup_distribution(0, POWERUP_TYPES - 1),
	timer_distribution(UFO_TIMER_MIN, UFO_TIMER_MAX),
	animation(UFO_ANIMATION_SPEED, 2 * BASE_SIZE, "Resources/Images/Ufo.png"),
	explosion(EXPLOSION_ANIMATION_SPEED, 2 * BASE_SIZE, "Resources/Images/ExplosionBig.png")
{
	reset(1, i_random_engine);

	for (unsigned char a = 0; a < POWERUP_TYPES; a++)
	{
		powerup_animations.push_back(Animation(POWERUP_ANIMATION_SPEED, BASE_SIZE, "Resources/Images/Powerup" + std::to_string(static_cast<unsigned short>(a)) + ".png"));
	}
}

bool Ufo::check_bullet_collision(std::mt19937_64& i_random_engine, const sf::IntRect& i_bullet_hitbox)
{
	if (0 == dead)
	{
		if (1 == get_hitbox().intersects(i_bullet_hitbox))
		{
			dead = 1;

			explosion_x = x;

			powerups.push_back(Powerup(x + 0.5f * BASE_SIZE, y, powerup_distribution(i_random_engine)));

			return 1;
		}
	}
	
	return 0;
}

unsigned char Ufo::check_powerup_collision(const sf::IntRect& i_player_hitbox)
{
	for (Powerup& powerup : powerups)
	{
		if (0 == powerup.dead && 1 == powerup.get_hitbox().intersects(i_player_hitbox))
		{
			powerup.dead = 1;

			//Plus 1, because 0 means we didn't pick up any powerups.
			return 1 + powerup.type;
		}
	}
	
	return 0;
}

void Ufo::draw(sf::RenderWindow& i_window)
{
	if (0 == dead)
	{
		animation.draw(x, y, i_window);
	}

	if (0 == dead_animation_over)
	{
		explosion.draw(explosion_x, y - 0.5f * BASE_SIZE, i_window, sf::Color(255, 36, 0));
	}

	for (Powerup& powerup : powerups)
	{
		powerup_animations[powerup.type].draw(powerup.x, powerup.y, i_window);
	}
}

void Ufo::reset(bool i_dead, std::mt19937_64& i_random_engine)
{
	dead = i_dead;
	dead_animation_over = 0;

	explosion_x = SCREEN_WIDTH;
	x = SCREEN_WIDTH;

	timer = timer_distribution(i_random_engine);

	powerups.clear();

	animation.reset();
	explosion.reset();
}

void Ufo::update(std::mt19937_64& i_random_engine)
{
	if (0 == dead)
	{
		x -= UFO_MOVE_SPEED;

		//As soon as the UFO leaves the screen, it gets destroyed. But no powerups will appear.
		if (x <= -2 * BASE_SIZE)
		{
			dead = 1;
		}

		animation.update();
	}
	else
	{
		if (0 == dead_animation_over)
		{
			dead_animation_over = explosion.update();
		}

		if (0 == timer)
		{
			reset(0, i_random_engine);
		}
		else
		{
			timer--;
		}
	}

	for (Powerup& powerup : powerups)
	{
		//Why didn't I made an update function for the powerups?
		//No, seriously.
		//I did it for the Bullet struct.
		//But not for the Powerup struct.
		powerup.y += POWERUP_SPEED;

		if (SCREEN_HEIGHT <= powerup.y)
		{
			powerup.dead = 1;
		}
	}

	for (Animation& powerup_animation : powerup_animations)
	{
		powerup_animation.update();
	}

	powerups.erase(remove_if(powerups.begin(), powerups.end(), [](const Powerup& i_powerup)
	{
		return 1 == i_powerup.dead;
	}), powerups.end());
}

sf::IntRect Ufo::get_hitbox() const
{
	return sf::IntRect(x, y, 2 * BASE_SIZE, BASE_SIZE);
}