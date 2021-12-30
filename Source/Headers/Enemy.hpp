#pragma once

class Enemy
{
	//-1 - Left
	//0 - Down
	//1 - Right
	char direction;

	unsigned char health;
	//The enemy will appear white for a few frames after being hit, so that the player knows about it.
	unsigned char hit_timer;
	//0 - Cyan one
	//1 - Purple one
	//2 - Green one
	unsigned char type;

	unsigned short x;
	unsigned short y;
public:
	Enemy(unsigned char i_type, unsigned short i_x, unsigned short i_y);

	unsigned char get_health() const;
	unsigned char get_hit_timer() const;
	unsigned char get_type() const;

	unsigned short get_x() const;
	unsigned short get_y() const;

	void hit();
	void move();
	void shoot(std::vector<Bullet>& i_enemy_bullets);
	void update();

	sf::IntRect get_hitbox() const;
};