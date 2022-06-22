#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

class TileMap : public sf::Drawable, public sf::Transformable {
	int width;
	int height;
	sf::VertexArray va;
	sf::View view;
	
public:
	TileMap(int, int);
	
	TileMap() = delete;
	
	TileMap(const TileMap&) = default;
	
	TileMap& operator=(const TileMap&) = default;
	
	void tile(int, int, int);
	void tile(int x, int y, int chr, bool h, bool v);
	void palette(int, int, int, int = 0);
	void clip(int, int, int, int);
	
	void draw(sf::RenderTarget&, sf::RenderStates) const;
};


