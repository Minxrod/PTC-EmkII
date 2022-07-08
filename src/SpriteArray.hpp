#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

#include "SpriteInfo.hpp"

/// Drawable collection of sprites.
/// 
/// This class manages a list of drawable sprites. Sprites can be added and moved, but not deleted.
class SpriteArray : public sf::Drawable, public sf::Transformable {
	/// VertexArray used for rendering sprites
	sf::VertexArray va{sf::Quads, 0};
	/// Map of sprite ids to sprite vertex ranges, used for updating vertices after a sprite was added.
	std::map<int, std::pair<int, int>> va_index; //id:[start_index, end_index)
	
public:
	/// Constructor
	SpriteArray();
	
	/// Adds a sprite to this SpriteArray.
	/// 
	/// @param s Sprite to add
	void add_sprite(const SpriteInfo& s);
	/// Updates the position of a sprite in this SpriteArray.
	/// @note Updates relative to current position.
	/// 
	/// @param s Sprite to update
	/// @param xofs Offset x location
	/// @param yofs Offset y location
	void update_sprite_xy(const SpriteInfo& s, int xofs, int yofs);
	
	/// Draws this collection of sprites.
	///
	/// @param r Target to draw to (ex. sf::RenderWindow)
	/// @param s States to draw with (includes shader, texture, etc.)
	void draw(sf::RenderTarget& r, sf::RenderStates s) const override;
};


