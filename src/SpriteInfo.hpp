#pragma once

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
		double dx = 0, dy = 0;
		int time = 0;
	} pos;
	
	struct {
		double s = 1.0;
		double ds = 0;
		int time = 0;
	} scale;
	
	struct {
		double a = 0.0;
		double da = 0;
		int time = 0;
	} angle;
	
	struct {
		double dx = 0, dy = 0;
		double x = 0, y = 0;
		double w = 0, h = 0;
		bool scale_adjust = 0;
		int mask = 0xff;
	} hit;
	
	struct {
		int chrs = 1;
		int frames_per_chr;
		int loop = 0;
		bool loop_forever = false;
		int current_frame = 0;
		int current_chr = 0;
	} anim;
	
	double vars[8];
};

/// Returns a SpriteInfo for the purposes of collision checks against a point.
/// 
/// @param x Location to check x
/// @param y Location to check y
/// @return SpriteInfo with small collision box and no data set for display
SpriteInfo make_touch_sprite(int x, int y);

/// Calculates if two sprites are colliding.
/// 
/// @param a Sprite 1
/// @param b Sprite 2
/// @return true if sprites are touching
bool is_hit(SpriteInfo& a, SpriteInfo& b);

