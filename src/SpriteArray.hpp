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
		int time;
	} scale;
	
	struct {
		double a = 0.0;
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
		int chrs = 1;
		int frames_per_chr;
		int loop = 0;
		bool loop_forever = false;
		int current_frame;
		int current_chr;
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
	std::map<int, std::pair<int, int>> va_index; //id:[start_index, end_index)
	
public:
	SpriteArray();
	
	void add_sprite(const SpriteInfo&);
	void update_sprite_xy(const SpriteInfo&, int, int);
	
	void draw(sf::RenderTarget&, sf::RenderStates) const;
};


