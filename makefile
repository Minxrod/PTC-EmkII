# https://www.gnu.org/software/make/manual/make.html
# https://makefiletutorial.com/
# https://stackoverflow.com/questions/14145344/makefile-wildcard-how-to-do-that-properly

SOURCE = src/
BUILD = build/
LIB = lib/
INCLUDE = include/
TESTS = tests/

# (Windows) Modify SFML_PATH if you have a different installation location.
SFML_PATH="C:/SFML-2.5.1/"
SFML_INCLUDE=$(SFML_PATH)include/
SFML_LIB=$(SFML_PATH)lib/

CXX = g++
OFLAGS = -g -O2 -std=c++17 -Wall -Werror -Wpedantic -Wextra -MMD -I$(INCLUDE) -L$(LIB) -Wl,-rpath,./lib

# https://stackoverflow.com/questions/714100/os-detecting-makefile
PLATFORM:=$(shell g++ -dumpmachine)
ifeq ($(PLATFORM),mingw32)
	OFLAGS += -I$(SFML_INCLUDE) -L$(SFML_LIB)
endif
ifeq ($(PLATFORM),i686-w64-mingw32)
	OFLAGS += -I$(SFML_INCLUDE) -L$(SFML_LIB)
endif

# https://stackoverflow.com/questions/24096807/dso-missing-from-command-line
CXXFLAGS = $(OFLAGS) -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system -lpthread -lSSEQPlayer
OBJECTS = BuiltinFuncs.o Evaluator.o Vars.o Tokens.o Variables.o md5.o Resources.o FileLoader.o PTC2Console.o Program.o Visual.o Input.o Sound.o Graphics.o Panel.o TileMap.o Background.o Sprites.o SpriteArray.o PanelKeyboard.o PTCSystem.o Icon.o ResourceTypes.o Debugger.o Repeater.o SpriteInfo.o Errors.o
TEST_OBJECTS = test_ptc_functions.o test_tokenize.o test_eval.o test_util.o

objs = $(OBJECTS:%=$(BUILD)%)
test_objs = $(TEST_OBJECTS:%=$(BUILD)%)

ptc: $(objs) $(BUILD)main.o
	$(CXX) -o ptc $(BUILD)main.o $(objs) $(CXXFLAGS)

$(BUILD)main.o: $(SOURCE)main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)%.o: $(SOURCE)%.cpp
	$(CXX) $(OFLAGS) -c $< -o $@

test: $(BUILD)tests.o $(test_objs) $(objs)
	$(CXX) -o test $(BUILD)tests.o $(test_objs) $(objs) $(CXXFLAGS)

$(BUILD)tests.o: $(TESTS)tests.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I$(SOURCE)

$(BUILD)test_%.o: $(TESTS)test_%.cpp
	$(CXX) $(OFLAGS) -c $< -o $@ -I$(SOURCE)

.PHONY: clean
clean:
	rm -f ptc
	rm -rf $(objs)
	rm -rf $(test_objs)
	rm -rf $(BUILD)main.o
	rm -rf $(BUILD)tests.o
	rm $(BUILD)*.d

# https://stackoverflow.com/questions/313778/generate-dependencies-for-a-makefile-for-a-project-in-c-c	
# https://stackoverflow.com/a/10168396
# this seems to be the simplest solution
-include $(objs:%.o=%.d)
-include $(test_objs:%.o=%.d)
