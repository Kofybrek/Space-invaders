#include <SFML/Graphics.hpp>

#include "Headers/DrawText.hpp"

void draw_text(short i_x, short i_y, const std::string& i_text, sf::RenderWindow& i_window, const sf::Texture& i_font_texture)
{
	short character_x = i_x;
	short character_y = i_y;

	unsigned char character_height = i_font_texture.getSize().y;
	//There are 96 characters in the font texture.
	unsigned char character_width = i_font_texture.getSize().x / 96;

	sf::Sprite character_sprite(i_font_texture);

	for (std::string::const_iterator a = i_text.begin(); a != i_text.end(); a++)
	{
		if ('\n' == *a)
		{
			character_x = i_x;
			character_y += character_height;

			continue;
		}

		character_sprite.setPosition(character_x, character_y);
		character_sprite.setTextureRect(sf::IntRect(character_width * (*a - 32), 0, character_width, character_height));

		character_x += character_width;

		i_window.draw(character_sprite);
	}
}