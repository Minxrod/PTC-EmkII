#include "SpriteInfo.hpp"

#include <vector>

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

vec normal(const SpriteInfo& s){
	if (s.hit.dx == 0 && s.hit.dy == 0)
		return vec{-s.pos.dy, s.pos.dx};
	return vec{-s.hit.dy, s.hit.dx};
}

vec colvec(const SpriteInfo& s){
	if (s.hit.dx == 0 && s.hit.dy == 0)
		return vec{s.pos.dx, s.pos.dy};
	return vec{s.hit.dx, s.hit.dy};
}

SpriteInfo make_touch_sprite(int x, int y){
	SpriteInfo touch_sprite{};
	touch_sprite.active = true;
	touch_sprite.w = 1;
	touch_sprite.h = 1;
	touch_sprite.pos.x = x;
	touch_sprite.pos.y = y;
	touch_sprite.hit.x = 0;
	touch_sprite.hit.y = 0;
	touch_sprite.hit.w = 2;
	touch_sprite.hit.h = 2;
	return touch_sprite;
}


bool is_hit(SpriteInfo& a, SpriteInfo& b){
	if (!a.active || !b.active)
		return false; //can't hit inactive sprites...
	
	if (!(a.hit.mask & b.hit.mask))
		return false; //can't hit different groups
		
	std::vector<vec> normals{
		vec{1, 0},
		vec{0, 1},
		normal(a).sub(normal(b))
	};
	std::vector<vec> a_verts{
		vec{a.pos.x, a.pos.y},
		vec{a.pos.x + a.hit.w, a.pos.y},
		vec{a.pos.x + a.hit.w, a.pos.y + a.hit.h},
		vec{a.pos.x, a.pos.y + a.hit.h},
	};
	std::vector<vec> b_verts{
		vec{b.pos.x, b.pos.y},
		vec{b.pos.x + b.hit.w, b.pos.y},
		vec{b.pos.x + b.hit.w, b.pos.y + b.hit.h},
		vec{b.pos.x, b.pos.y + b.hit.h},
	};
	
	for (int i = 0; i < 4; ++i){
		a_verts.push_back(a_verts[i].add(colvec(a)).sub(colvec(b)));
	}
	
	//u.v/u.u
	bool is_hit = true;
	for (auto n : normals){
		if (n.x == 0 && n.y == 0){ //don't project to zero vector...
			continue;
		}
		//determine projected range of sprite
		double min = 1e300;
		double max = -1e300;
		for (auto v : b_verts){
			double d = n.dot(v);
			
			if (d < min)
				min = d;
			if (d > max)
				max = d;
		}
		//check for presence of other sprite in range of first
		bool in_range = false;
		bool on_left = false;
		bool on_right = false;
		for (auto v : a_verts){
			double d = n.dot(v);
			if (d < min){
				on_left = true;
			}
			if (d > max){
				on_right = true;
			}
			if (min <= d && d <= max){
				in_range = true;
				break;
			}
			if (on_left && on_right){
				in_range = true;
				break;
			}
		}
		//found separating axis if sprites do not overlap
		if (!in_range){
			is_hit = false;
			break;
		}
	}
	
	return is_hit;
}

