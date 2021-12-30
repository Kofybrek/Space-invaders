#include <array>
#include <chrono>
#include <random>
#include <SFML/Graphics.hpp>

#include "Headers/Animation.hpp"
#include "Headers/Global.hpp"
#include "Headers/Enemy.hpp"
#include "Headers/Ufo.hpp"
#include "Headers/Player.hpp"

Player::Player() :
	explosion(EXPLOSION_ANIMATION_SPEED, BASE_SIZE, "Resources/Images/Explosion.png")
{
	reset();

	bullet_texture.loadFromFile("Resources/Images/PlayerBullet.png");
	texture.loadFromFile("Resources/Images/Player.png");

	bullet_sprite.setTexture(bullet_texture);
	sprite.setTexture(texture);
}

bool Player::get_dead() const
{
	return dead;
}

bool Player::get_dead_animation_over() const
{
	return dead_animation_over;
}

unsigned char Player::get_current_power() const
{
	return current_power;
}

unsigned short Player::get_power_timer() const
{
	return power_timer;
}

unsigned short Player::get_y() const
{
	return y;
}

//I don't know why, but this is funny.
void Player::die()
{
	dead = 1;
}

void Player::draw(sf::RenderWindow& i_window)
{
	if (0 == dead)
	{
		sprite.setPosition(x, y);
		sprite.setTextureRect(sf::IntRect(BASE_SIZE * current_power, 0, BASE_SIZE, BASE_SIZE));

		for (const Bullet& bullet : bullets)
		{
			bullet_sprite.setPosition(bullet.x, bullet.y);

			i_window.draw(bullet_sprite);
		}

		i_window.draw(sprite);

		if (0 == shield_animation_over)
		{
			//Once we get hit while having a shield, the shield will be destroyed. We'll show a blue explosion.
			explosion.draw(x, y, i_window, sf::Color(0, 109, 255));
		}
	}
	else if (0 == dead_animation_over)
	{
		explosion.draw(x, y, i_window, sf::Color(255, 36, 0));
	}
}

void Player::reset()
{
	dead = 0;
	dead_animation_over = 0;
	shield_animation_over = 1;

	current_power = 0;
	reload_timer = 0;

	power_timer = 0;
	x = 0.5f * (SCREEN_WIDTH - BASE_SIZE);
	y = SCREEN_HEIGHT - 2 * BASE_SIZE;

	bullets.clear();

	explosion.reset();
}

void Player::update(std::mt19937_64& i_random_engine, std::vector<Bullet>& i_enemy_bullets, std::vector<Enemy>& i_enemies, Ufo& i_ufo)
{
	if (0 == dead)
	{
		unsigned char powerup_type;

		if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			if (4 == current_power)
			{
				//Mirrored controls power-DOWN!
				x = std::min<int>(PLAYER_MOVE_SPEED + x, SCREEN_WIDTH - 2 * BASE_SIZE);
			}
			else
			{
				x = std::max<int>(x - PLAYER_MOVE_SPEED, BASE_SIZE);
			}
		}

		if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			if (4 == current_power)
			{
				//Mirrored controls power-DOWN!
				//I'm never gonna get tired of this joke.
				//NEVER!
				x = std::max<int>(x - PLAYER_MOVE_SPEED, BASE_SIZE);
			}
			else
			{
				x = std::min<int>(PLAYER_MOVE_SPEED + x, SCREEN_WIDTH - 2 * BASE_SIZE);
			}
		}

		if (0 == reload_timer)
		{
			if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
			{
				if (2 == current_power)
				{
					reload_timer = FAST_RELOAD_DURATION;
				}
				else
				{
					reload_timer = RELOAD_DURATION;
				}

				bullets.push_back(Bullet(0, -PLAYER_BULLET_SPEED, x, y));

				if (3 == current_power)
				{
					bullets.push_back(Bullet(0, -PLAYER_BULLET_SPEED, x - 0.375f * BASE_SIZE, y));
					bullets.push_back(Bullet(0, -PLAYER_BULLET_SPEED, x + 0.375f * BASE_SIZE, y));
				}
			}
		}
		else
		{
			reload_timer--;
		}

		for (Bullet& enemy_bullet : i_enemy_bullets)
		{
			if (1 == get_hitbox().intersects(enemy_bullet.get_hitbox()))
			{
				if (1 == current_power)
				{
					current_power = 0;

					shield_animation_over = 0;
				}
				else
				{
					dead = 1;
				}

				enemy_bullet.dead = 1;

				break;
			}
		}

		powerup_type = i_ufo.check_powerup_collision(get_hitbox());

		if (0 < powerup_type)
		{
			current_power = powerup_type;

			power_timer = POWERUP_DURATION;
		}

		if (0 == power_timer)
		{
			current_power = 0;
		}
		else
		{
			power_timer--;
		}

		if (0 == shield_animation_over)
		{
			shield_animation_over = explosion.update();
		}
	}
	else if (0 == dead_animation_over)
	{
		dead_animation_over = explosion.update();
	}

	for (Bullet& bullet : bullets)
	{
		bullet.update();
		
		if (0 == bullet.dead)
		{
			if (1 == i_ufo.check_bullet_collision(i_random_engine, bullet.get_hitbox()))
			{
				bullet.dead = 1;
			}
		}
	}

	for (Enemy& enemy : i_enemies)
	{
		for (Bullet& bullet : bullets)
		{
			if (0 == bullet.dead && 0 < enemy.get_health() && 1 == enemy.get_hitbox().intersects(bullet.get_hitbox()))
			{
				bullet.dead = 1;

				enemy.hit();

				break;
			}
		}
	}

	bullets.erase(remove_if(bullets.begin(), bullets.end(), [](const Bullet& i_bullet)
	{
		return 1 == i_bullet.dead;
	}), bullets.end());
}

sf::IntRect Player::get_hitbox() const
{
	return sf::IntRect(x + 0.125f * BASE_SIZE, y + 0.125f * BASE_SIZE, 0.75f * BASE_SIZE, 0.75f * BASE_SIZE);
}