#include <SFML/Graphics.hpp>

#include <string>

#include "PTCSystem.hpp"

// search for command line option
bool option(std::string option, int option_count, char** options){
	for (int i = 0; i < option_count; ++i)
		if (std::string(options[i]) == std::string(option))
			return true;
	return false;
}

// search for command line string
// must be of form "-<opt> <argument>"
std::string option_arg(std::string option, int option_count, char** options){
	for (int i = 0; i < option_count; ++i)
		if (std::string(options[i]) == option)
			return std::string(options[i+1]);
	throw std::runtime_error{"Err: Option argument not provided"};
}


int main(int argc, char**argv){
	std::string file = ""; //default runs the loader program
	if (option("-p",argc,argv)){
		file = option_arg("-p",argc,argv);
	}
	PTCSystemDisplay system{file};
	
	system.set_option("-s", !option("-s",argc,argv));
	system.set_option("-a", option("-a",argc,argv));
	if (option("-d",argc,argv))
		system.set_option("-d", true);
	
	system.start();
	
	while (system.is_ok()){
		system.update();
	}
	
	return 0;
}
