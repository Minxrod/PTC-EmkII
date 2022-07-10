#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

/// Drawable tilemap class.
/// 
/// Used for a grid of renderable tiles, such as a console or BG layer. 
/// 
/// @note Only intended to be drawn with the palette shader.
class TileMap : public sf::Drawable, public sf::Transformable {
	/// Width of this tilemap, in tiles
	int width;
	/// Height of this tilemap, in tiles
	int height;
	/// VertexArray to store the polygons used by SFML for rendering.
	sf::VertexArray va;
	/// View used to set clipping bounds.
	sf::View view;
	
public:
	/// Constructor
	/// 
	/// @param w Width of map
	/// @param h Width of map
	TileMap(int w, int h);
	
	/// Default constructor (deleted)
	TileMap() = delete;
	
	/// Copy constructor (default)
	TileMap(const TileMap&) = default;
	
	/// Copy assignment (default)
	TileMap& operator=(const TileMap&) = default;
	
	/// Sets a tile with the given information.
	/// 
	/// @param x Tile x location
	/// @param y Tile y location
	/// @param chr Character code
	void tile(int x, int y, int chr);
	/// Sets a tile with the given information.
	/// 
	/// @param x Tile x location
	/// @param y Tile y location
	/// @param chr Character code
	/// @param h true=Horizontally flip texture
	/// @param v true=Vertically flip texture
	void tile(int x, int y, int chr, bool h, bool v);
	/// Sets the palette of the tile at (x,y).
	/// 
	/// @param x Tile x location
	/// @param y Tile y location
	/// @param pal Palette number
	/// @param bg Background color (default=0)
	void palette(int x, int y, int pal, int bg = 0);
	
	/// Draws this tilemap.
	///
	/// @param t Target to draw to (ex. sf::RenderWindow)
	/// @param s States to draw with (includes shader, texture, etc.)
	void draw(sf::RenderTarget& t, sf::RenderStates s) const override;
};


