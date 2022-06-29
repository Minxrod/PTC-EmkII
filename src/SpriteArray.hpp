#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

struct SpriteInfo {
	bool active;
	int id;
	int chr, pal;
	int w, h;
	int prio;
	bool flip_x = false, flip_y = false;

	double home_x = 0, home_y = 0;
	
	struct {
		double x = 0, y = 0;
		double dx,dy;
		int time = 0;
	} pos;
	
	struct {
		double s = 1.0;
		double ds;
		int time = 0;
	} scale;
	
	struct {
		double a = 0.0;
		double da;
		int time = 0;
	} angle;
	
	struct {
		double dx, dy;
		double x, y, w, h;
		bool scale_adjust;
		int mask = 0xff;
	} hit;
	
	struct {
		int chrs = 1;
		int frames_per_chr;
		int loop = 0;
		bool loop_forever = false;
		int current_frame;
		int current_chr;
	} anim;
	
	double vars[8];
};

/// Simple vector class
struct vec {
	/// x component of vector.
	double x;
	/// y component of vector.
	double y;
	
	/// Calculate dot product of this vector and a second vector.
	/// 
	/// @param other Vector to dot with
	/// @return Result of dot product
	double dot(const vec& other){
		return x * other.x + y * other.y;
	}
	
	/// Adds this vector and another.
	/// 
	/// @param other Secnod vector to add
	/// @returns Sum of vectors
	vec add(const vec& other){
		return vec{x + other.x, y + other.y};
	}
	
	/// Subtracts a vector from this vector.
	/// 
	/// @param other Vector to subtract
	/// @returns Difference of this vector with other
	vec sub(const vec& other){
		return vec{x - other.x, y - other.y};
	}
};

/// Calculates if two sprites are colliding.
/// 
/// @param a Sprite 1
/// @param b Sprite 2
/// @return true if sprites are touching
bool is_hit(SpriteInfo& a, SpriteInfo& b);

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


