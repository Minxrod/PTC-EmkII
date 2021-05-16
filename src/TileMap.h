#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

class TileMap : public sf::Drawable, public sf::Transformable {
	int width;
	int height;
//	sf::Texture tex;
	sf::VertexArray va;
	
public:
	TileMap(int, int/*, sf::Texture*/);
	
	void tile(int, int, int);
	void palette(int, int, int, int);
	
	void draw(sf::RenderTarget&, sf::RenderStates) const;
};


