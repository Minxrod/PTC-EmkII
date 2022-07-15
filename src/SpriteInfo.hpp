#pragma once

/// Contains all the information for one sprite.
/// 
/// This includes the sprite's visuals, animation, position and movement, sprite variables, hitbox, etc.
struct SpriteInfo {
	/// Is the sprite currently enabled?
	bool active;
	/// Sprite ID (control number)
	int id;
	/// Character code [0-511]
	int chr;
	/// Palette [0-15]
	int pal;
	/// Width of sprite (pixels)
	int w;
	/// Height of sprite (pixels)
	int h;
	/// Sprite draw priority [0-3]
	int prio;
	/// Is sprite flipped horizontally?
	bool flip_x = false;
	/// Is sprite flipper vertically?
	bool flip_y = false;
	
	/// Sprite home x component (pixels)
	double home_x = 0;
	/// Sprite home y component (pixels)
	double home_y = 0;
	
	/// Struct for sprite position.
	struct {
		/// Sprite current x position (pixels)
		double x = 0;
		/// Sprite current y position (pixels)
		double y = 0;
		/// Change in x position (pixels/frame)
		double dx = 0;
		/// Change in y position (pixels/frame)
		double dy = 0;
		/// Remaining time to move sprite (frames)
		/// If 0, sprite is done moving.
		int time = 0;
	} pos;
	
	/// Struct for sprite scaling.
	struct {
		/// Current scale multiplier [0.0-2.0]
		double s = 1.0;
		/// Change in scale over time (step/frame)
		double ds = 0;
		/// Remaining time to scale sprite (frames)
		/// If 0, sprite is done resizing.
		int time = 0;
	} scale;
	
	/// Struct for sprite rotation
	struct {
		/// Current sprite angle (degrees)
		double a = 0.0;
		/// Change in angle (degrees/frame)
		double da = 0;
		/// Remaining time rotate sprite (frames)
		/// If 0, sprite is done rotating.
		int time = 0;
	} angle;
	
	/// Struct for sprite collision detection.
	struct {
		/// Displacement x (pixels)
		double dx = 0;
		/// Displacement y (pixels)
		double dy = 0;
		/// Hitbox x coordinate (pixels)
		double x = 0;
		/// Hitbox y coordinate (pixels)
		double y = 0;
		/// Width of hitbox (pixels)
		double w = 0;
		/// Height of hitbox (pixels)
		double h = 0;
		/// Should hitbox change with sprite scale?
		bool scale_adjust = false;
		/// Collision mask
		int mask = 0xff;
	} hit;
	
	/// Struct for sprite animation
	struct {
		/// Number of characters to animate through (number of animation frames)
		int chrs = 1;
		/// Time to stay on one character (frames)
		int frames_per_chr;
		/// How many times remaining to loop
		int loop = 0;
		/// Should animation loop forever?
		bool loop_forever = false;
		/// Current animation frame
		int current_frame = 0;
		/// Current animation character
		int current_chr = 0;
	} anim;
	
	/// Sprite variables (used by `SPSETV`, `SPGETV`)
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

