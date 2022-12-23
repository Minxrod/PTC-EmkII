#pragma once
#include <iostream>

class Logger {
public:
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
	
	int debug_on = -1; //bitmask
	int debug_level = 5; //0= none 5=all
	
	Logger() = default;
	virtual ~Logger(){}
	
	virtual std::string debug_name() = 0;
	virtual int debug_mask() = 0;
	
	template <typename T, int level>
	void common(std::ostream& os, const char* info, T message){
		if ((debug_on & debug_mask()) && debug_level > level){
		 os	<< info 
			<< "[" << debug_name() <<"]	"
 			<< message
 			<< std::endl;
		}
	}
	
	template <typename T>
	void debug(T message) { common<T,4>(std::cout, "[DEBUG]	", message); }
	
	template <typename T>
	void log(T message) { common<T,3>(std::cout, "[LOG]	", message); }
	
	template <typename T>
	void info(T message) { common<T,2>(std::cout, "[INFO]	", message); }
	
	template <typename T>
	void warn(T message) { common<T,1>(std::cout, "[WARN]	", message); }
	
	template <typename T>
	void error(T message) { 
//		common<T,0>(std::cout, "[ERROR]	", message);
		common<T,0>(std::cerr, "[ERROR]	", message);
	}
};
