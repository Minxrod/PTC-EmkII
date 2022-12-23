#pragma once
#include <iostream>

namespace logger {
	const int DEBUG_BACKGROUND = (1<<0);
	const int DEBUG_CONSOLE = (1<<1);
	const int DEBUG_FUNCTIONS = (1<<2);
	const int DEBUG_PTC_DEBUGGER = (1<<3);
	const int DEBUG_EVALUATOR = (1<<4);
	const int DEBUG_FILES = (1<<5);
	const int DEBUG_GRAPHICS = (1<<6);
	const int DEBUG_ICON = (1<<7);
	const int DEBUG_INPUT = (1<<8);
	const int DEBUG_OPTIONS = (1<<9);
	const int DEBUG_PANEL = (1<<10);
	const int DEBUG_KEYBOARD = (1<<11);
	const int DEBUG_SYSTEM = (1<<12);
	const int DEBUG_RESOURCES = (1<<13);
	const int DEBUG_SOUND = (1<<14);
	const int DEBUG_SPRITE = (1<<15);
	const int DEBUG_VARIABLES = (1<<16);
	const int DEBUG_VISUAL = (1<<17);
	const int DEBUG_PROGRAM = (1<<18);

//	int debug_on = -1; //bitmask
	const int debug_level = 5; //0= none 5=all

	template <typename T, int level>
	void common(std::ostream& os, std::string info, const T& message){
		if (debug_level > level){
		 os	<< info
			<< message
			<< std::endl;
		}
	}
	
	template <typename T>
	void debug(std::string name, const T& message) { common<T,4>(std::cout, "[DEBUG]	["+name+"]", message); }
	
	template <typename T>
	void log(std::string name, const T& message) { common<T,3>(std::cout, "[LOG]	["+name+"]", message); }
	
	template <typename T>
	void info(std::string name, const T& message) { common<T,2>(std::cout, "[INFO]	["+name+"]", message); }
	
	template <typename T>
	void warn(std::string name, const T& message) { common<T,1>(std::cout, "[WARN]	["+name+"]", message); }
	
	template <typename T>
	void error(std::string name, const T& message) { common<T,0>(std::cerr, "[ERROR]	["+name+"]", message); }
}
