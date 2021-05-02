# https://www.gnu.org/software/make/manual/make.html
# https://makefiletutorial.com/
# https://stackoverflow.com/questions/14145344/makefile-wildcard-how-to-do-that-properly

SOURCE = src/
BUILD = build/

CXX = g++
OFLAGS = -std=c++17 -Wall -Werror -Wpedantic -Wextra -MMD
CXXFLAGS = $(OFLAGS) -lsfml-graphics -lsfml-window -lsfml-system
OBJECTS = main.o Evaluator.o Vars.o Resources.o FileLoader.o Console.o Program.o

objs = $(OBJECTS:%=$(BUILD)%)

ptc: $(objs)
	$(CXX) -g -o ptc $(objs) $(CXXFLAGS)

$(BUILD)main.o: $(SOURCE)main.cpp
	$(CXX) $(CXXFLAGS) -g -c $< -o $@

$(BUILD)%.o: $(SOURCE)%.cpp 
	$(CXX) $(OFLAGS) -g -c $< -o $@

.PHONY: clean
clean:
	rm -f ptc
	rm -r $(objs)

# https://stackoverflow.com/questions/313778/generate-dependencies-for-a-makefile-for-a-project-in-c-c	
# https://stackoverflow.com/a/10168396
# this seems to be the simplest solution
-include $(objs:%.o=%.d)
