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

	double home_x, home_y;
	
	struct {
		double x, y;
		double dx,dy;
		int time;
	} pos;
	
	struct {
		double s = 1.0;
		double ds;
		int time;
	} scale;
	
	struct {
		double a;
		double da;
		int time;
	} angle;
	
	struct {
		double dx, dy;
		double x, y, w, h;
		bool scale_adjust;
		int mask = 0xff;
	} hit;
	
	struct {
		int chr, frames, time;
		int loop;
	} anim;
	
	double vars[8];
};

struct vec {
	double x, y;
	
	double dot(const vec& other){
		return x * other.x + y * other.y;
	}
	
	vec add(const vec& other){
		return vec{x + other.x, y + other.y};
	}
	
	vec sub(const vec& other){
		return vec{x - other.x, y - other.y};
	}
};

//vec normal(const SpriteInfo&);

bool is_hit(SpriteInfo&, SpriteInfo&);

class SpriteArray : public sf::Drawable, public sf::Transformable {
	sf::VertexArray va{sf::Quads, 0};
	
public:
	SpriteArray();
	
	void add_sprite(const SpriteInfo&);
	
	void draw(sf::RenderTarget&, sf::RenderStates) const;
};


