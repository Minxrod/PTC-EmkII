#pragma once

/// Simple struct to handle repeating frame timers
/// 
/// Used for keyboard, icon, buttons, etc.
struct Repeater {
	/// Current time (frames)
	int current_time = 0;
	/// Start time for repetition (frames)
	int start_time = 0;
	/// Repeat interval (after start time, frames)
	int repeat_interval = 0;
	
	/// Sets up the repeater with default timings. (Used for keyboard and icon)
	/// 
	/// @param start Start time to begin repeating (frames)
	/// @param repeat Interval to repeat at (frames)
	Repeater(int start = 15, int repeat = 4);
	
	/// Advances timer by one frame
	void advance();
	/// Resets timer to zero.
	void reset();
	/// Checks if the repeat is currently triggered.
	/// @note This returns true on frame 1 in addition to the usual repeat timings.
	///
	/// @return true if repeating
	bool check() const;
};
