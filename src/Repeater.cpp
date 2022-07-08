#include "Repeater.hpp"

Repeater::Repeater(int start, int repeat) : current_time{0}, start_time{start}, repeat_interval{repeat}
{}

void Repeater::advance(){
	current_time++;
}

void Repeater::reset(){
	current_time = 0;
}

bool Repeater::check() const {
	if (repeat_interval > 0){ //repeat=0 -> disabled
		if (current_time > start_time){ //repeat only applies past start time
			return ((current_time - start_time) % (repeat_interval+1)) == 0;
		}
	}
	return current_time == 1;
}
