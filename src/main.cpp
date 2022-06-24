#include <SFML/Graphics.hpp>

#include "PTCSystem.hpp"

// search for command line option
bool option(std::string option, int option_count, char** options){
	for (int i = 0; i < option_count; ++i)
		if (std::string(options[i]) == std::string(option))
			return true;
	return false;
}

int main(int argc, char**argv){
	PTCSystem system{};
	
	system.set_option("-s", !option("-s",argc,argv));
	system.set_option("-a", option("-a",argc,argv));
	if (option("-d",argc,argv))
		system.set_option("-d", true);
	
	system.start();
	
    while (system.is_ok())
    {
    	system.update();
	}
	
	return 0;
}
