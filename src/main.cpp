#include <SFML/Graphics.hpp>

#include "PTCSystem.hpp"

#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <map>

// search for command line option
bool option(std::string option, int option_count, char** options){
	for (int i = 0; i < option_count; ++i)
		if (std::string(options[i]) == std::string(option))
			return true;
	return false;
}

int main(int argc, char**argv){
	PTCSystem system{};
	
	//options:
	//-s: sound disable
	system.set_option("-s", !option("-s",argc,argv));
	
	system.start();
	
    while (system.is_ok())
    {
    	system.update();
	}
	
	return 0;
}
