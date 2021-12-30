#include <array>
#include <chrono>
#include <random>
#include <SFML/Graphics.hpp>

#include "Headers/Animation.hpp"
#include "Headers/Global.hpp"
#include "Headers/Enemy.hpp"
#include "Headers/EnemyManager.hpp"

EnemyManager::EnemyManager() :
	shoot_distribution(0, ENEMY_SHOOT_CHANCE)
{
	//We have a function that sets everything to the initial state, so why not use it?
	reset(0);

	enemy_bullet_texture.loadFromFile("Resources/Images/EnemyBullet.png");

	enemy_bullet_sprite.setTexture(enemy_bullet_texture);

	for (unsigned char a = 0; a < ENEMY_TYPES; a++)
	{
		enemy_animations.push_back(Animation(1 + move_pause, BASE_SIZE, "Resources/Images/Enemy" + std::to_string(static_cast<unsigned short>(a)) + ".png"));
	}
}

bool EnemyManager::reached_player(unsigned short i_player_y) const
{
	for (const Enemy& enemy : enemies)
	{
		if (enemy.get_y() > i_player_y - 0.5f * BASE_SIZE)
		{
			//As soon as the enemies reach the player, the game is over!
			return 1;
		}
	}

	return 0;
}

void EnemyManager::draw(sf::RenderWindow& i_window)
{
	for (const Bullet& bullet : enemy_bullets)
	{
		//Drawing the tail of the bullet.
		for (unsigned char a = 0; a < bullet.previous_x.size(); a++)
		{
			enemy_bullet_sprite.setPosition(bullet.previous_x[a], bullet.previous_y[a]);
			enemy_bullet_sprite.setTextureRect(sf::IntRect(BASE_SIZE * a, 0, BASE_SIZE, BASE_SIZE));

			i_window.draw(enemy_bullet_sprite);
		}

		//Drawing the bullet itself.
		enemy_bullet_sprite.setPosition(bullet.x, bullet.y);
		enemy_bullet_sprite.setTextureRect(sf::IntRect(BASE_SIZE * bullet.previous_x.size(), 0, BASE_SIZE, BASE_SIZE));

		i_window.draw(enemy_bullet_sprite);
	}

	for (Enemy& enemy : enemies)
	{
		//When the enemy gets hit, it's gonna appear white.
		sf::Color enemy_color = sf::Color(255, 255, 255);

		if (0 == enemy.get_hit_timer())
		{
			//Otherwise, we're gonna color it.
			switch (enemy.get_type())
			{
				case 0:
				{
					enemy_color = sf::Color(0, 255, 255);

					break;
				}
				case 1:
				{
					enemy_color = sf::Color(255, 0, 255);

					break;
				}
				case 2:
				{
					enemy_color = sf::Color(0, 255, 0);
				}
			}
		}

		enemy_animations[enemy.get_type()].draw(enemy.get_x(), enemy.get_y(), i_window, enemy_color);
	}
}

void EnemyManager::reset(unsigned short i_level)
{
	//RESET EVERYTHING!!!!
	unsigned char enemy_x = 0;
	unsigned char enemy_y = 0;

	std::string level_sketch = "";

	move_pause = std::max<short>(ENEMY_MOVE_PAUSE_START_MIN, ENEMY_MOVE_PAUSE_START - ENEMY_MOVE_PAUSE_DECREASE * i_level);
	move_timer = move_pause;

	shoot_distribution = std::uniform_int_distribution<unsigned short>(0, std::max<short>(ENEMY_SHOOT_CHANCE_MIN, ENEMY_SHOOT_CHANCE - ENEMY_SHOOT_CHANCE_INCREASE * i_level));

	for (Animation& enemy_animation : enemy_animations)
	{
		enemy_animation.reset();
	}

	enemy_bullets.clear();

	enemies.clear();

	//There are 8 levels. Once the player finishes level 8, we go back to level 4. This is the same thing we did in the game "Frogger".
	//Go watch that video, btw!
	if (TOTAL_LEVELS <= i_level)
	{
		i_level = 0.5f * TOTAL_LEVELS + i_level % static_cast<unsigned char>(0.5f * TOTAL_LEVELS);
	}

	//Here you can see my pro level design skills!
	switch (i_level)
	{
		case 0:
		{
			level_sketch = "0 0 0 0 0 0 0 0 \n 0 0 0 0 0 0 0 0\n0 0 0 0 0 0 0 0 \n 0 0 0 0 0 0 0 0";

			break;
		}
		case 1:
		{
			level_sketch = "0000000000000000\n0000000000000000\n0000000000000000\n0000000000000000";

			break;
		}
		case 2:
		{
			//OH MY GOD, IS THAT A CHECKBOARD PATTERN?!
			level_sketch = "1010101010101010\n0101010101010101\n1010101010101010\n0101010101010101";

			break;
		}
		case 3:
		{
			level_sketch = "1111111111111111\n1111111111111111\n1111111111111111\n1111111111111111";

			break;
		}
		case 4:
		{
			//CHECKBOARD PATTERN WITH A SOLID PATTERN! I'M A GENIUS!
			level_sketch = "2222222222222222\n1111111111111111\n1010101010101010\n0101010101010101";

			break;
		}
		case 5:
		{
			//EASY ENEMIES AT THE TOP AND HARD ENEMIES AT THE BOTTOM! IT'S A REVOLUTION IN LEVEL DESIGN!
			level_sketch = "0000000000000000\n2222222222222222\n1111111111111111\n1111111111111111";

			break;
		}
		case 6:
		{
			//CHECKBOARD PATTERN AGAIN?!
			level_sketch = "2121212121212121\n1212121212121212\n2121212121212121\n1212121212121212";

			break;
		}
		case 7:
		{
			level_sketch = "2222222222222222\n2222222222222222\n2222222222222222\n2222222222222222";
		}
	}

	//Here we're converting each character into an enemy.
	for (char sketch_character : level_sketch)
	{
		enemy_x++;

		switch (sketch_character)
		{
			case '\n':
			{
				enemy_x = 0;
				enemy_y++;

				break;
			}
			case '0':
			{
				enemies.push_back(Enemy(0, BASE_SIZE * (1 + enemy_x), BASE_SIZE * (2 + enemy_y)));

				break;
			}
			case '1':
			{
				enemies.push_back(Enemy(1, BASE_SIZE * (1 + enemy_x), BASE_SIZE * (2 + enemy_y)));

				break;
			}
			case '2':
			{
				enemies.push_back(Enemy(2, BASE_SIZE * (1 + enemy_x), BASE_SIZE * (2 + enemy_y)));
			}
		}
	}
}

void EnemyManager::update(std::mt19937_64& i_random_engine)
{
	std::vector<Enemy>::iterator dead_enemies_start;

	if (0 == move_timer)
	{
		move_timer = move_pause;

		for (Enemy& enemy : enemies)
		{
			enemy.move();
		}

		for (Animation& enemy_animation : enemy_animations)
		{
			//The enemies change their frame after each move.
			enemy_animation.change_current_frame();
		}
	}
	else
	{
		move_timer--;
	}

	for (Enemy& enemy : enemies)
	{
		enemy.update();

		if (0 == shoot_distribution(i_random_engine))
		{
			enemy.shoot(enemy_bullets);
		}
	}

	//I used a lambda!
	//I'm a pro!
	//No, not like that.
	//I'M A PROFESSIONAL C++ PROGRAMMER!!!!
	//Yeah, that's better.
	dead_enemies_start = remove_if(enemies.begin(), enemies.end(), [](const Enemy& i_enemy)
	{
		return 0 == i_enemy.get_health();
	});

	//The more enemies we kill, the faster they become.
	move_pause = std::max<int>(ENEMY_MOVE_PAUSE_MIN, move_pause - ENEMY_MOVE_PAUSE_DECREASE * (enemies.end() - dead_enemies_start));

	enemies.erase(dead_enemies_start, enemies.end());

	for (Bullet& enemy_bullet : enemy_bullets)
	{
		enemy_bullet.update();
	}

	//I used a lambda!
	//AGAIN!
	enemy_bullets.erase(remove_if(enemy_bullets.begin(), enemy_bullets.end(), [](const Bullet& i_bullet)
	{
		return 1 == i_bullet.dead;
	}), enemy_bullets.end());
}

//Yes, that's a reference return type.
std::vector<Bullet>& EnemyManager::get_enemy_bullets()
{
	return enemy_bullets;
}

std::vector<Enemy>& EnemyManager::get_enemies()
{
	return enemies;
}