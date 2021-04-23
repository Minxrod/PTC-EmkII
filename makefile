# https://www.gnu.org/software/make/manual/make.html#Reading
# https://makefiletutorial.com/

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Werror -Wpedantic -Wextra -lsfml-graphics -lsfml-window -lsfml-system
objs = main.o Evaluator.o Vars.o Resources.o FileLoader.o
srcs = src/main.cpp src/Evaluator.cpp src/Vars.cpp src/FileLoader.cpp src/Resources.cpp

ptc: $(objs)
	$(CXX) $(CXXFLAGS) -o ptc $(objs)
	
$(objs): $(srcs)
	$(CXX) -std=c++17 -Wall -Werror -Wpedantic -Wextra -c $(srcs) -g

.PHONY: clean
clean:
	rm -f ptc
	rm -r $(objs)
