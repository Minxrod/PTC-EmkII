#include "catch.hpp"
#include "test_util.hpp"

#include <thread>
#include <iostream>

#include "PTCSystem.hpp"

const double SMALL = 1.0/4096.0;

const std::string test_prg_1{"\
SPCLR\r\
DIM R[100]\r\
I=0\r\
SPSET 0,0,0,0,0,0\r\
\r\
SPOFS 0,200,100,30\r\
@OFS\r\
SPREAD(0),X,Y\r\
?X,Y\r\
R[I] = X:I=I+1:R[I]=Y:I=I+1\r\
WAIT 5\r\
IF SPCHK(0) THEN @OFS\r\
\r"};

TEST_CASE("Sprite offset test", "sprites"){
	auto tokens = tokenize_str(test_prg_1);
	
	// set tokens as program and call run_();
	PTCSystem system;
	system.get_program()->set_tokens(tokens);
	system.start();
	while (system.is_running()){
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000.0/60.0)));
		system.update();
	}
//	debug_print(system.get_visual()->c);
	
	CHECK(debug_eval(system, "R[0]") == Approx(0).margin(SMALL));
	CHECK(debug_eval(system, "R[1]") == Approx(0).margin(SMALL));
	CHECK(debug_eval(system, "R[2]") == Approx(33).margin(SMALL));
	CHECK(debug_eval(system, "R[3]") == Approx(16).margin(SMALL));
	CHECK(debug_eval(system, "R[4]") == Approx(66).margin(SMALL));
	CHECK(debug_eval(system, "R[5]") == Approx(33).margin(SMALL));
	CHECK(debug_eval(system, "R[6]") == Approx(100).margin(SMALL));
	CHECK(debug_eval(system, "R[7]") == Approx(50).margin(SMALL));
	CHECK(debug_eval(system, "R[8]") == Approx(133).margin(SMALL));
	CHECK(debug_eval(system, "R[9]") == Approx(66).margin(SMALL));
	CHECK(debug_eval(system, "R[10]") == Approx(166).margin(SMALL));
	CHECK(debug_eval(system, "R[11]") == Approx(83).margin(SMALL));
	CHECK(debug_eval(system, "R[12]") == Approx(200).margin(SMALL));
	CHECK(debug_eval(system, "R[13]") == Approx(100).margin(SMALL));
}

const std::string test_prg_2{"\
SPCLR\r\
DIM R[100]\r\
I=0\r\
SPSET 0,0,0,0,0,0\r\
\r\
SPOFS 0,200,100,0\r\
SPOFS 0,0,0,30\r\
@OFS\r\
SPREAD(0),X,Y\r\
?X,Y\r\
R[I] = X:I=I+1:R[I]=Y:I=I+1\r\
WAIT 5\r\
IF SPCHK(0) THEN @OFS\r\
\r"};

// Test immediate SPOFS using time=0
// Test interpolation works in both directions
TEST_CASE("Sprite offset test 2", "sprites"){
	auto tokens = tokenize_str(test_prg_2);
	
	// set tokens as program and call run_();
	PTCSystem system;
	system.get_program()->set_tokens(tokens);
	system.start();
	while (system.is_running()){
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000.0/60.0)));
		system.update();
	}
//	debug_print(system.get_visual()->c);
	
	CHECK(debug_eval(system, "R[0]") == Approx(200).margin(SMALL));
	CHECK(debug_eval(system, "R[1]") == Approx(100).margin(SMALL));
	CHECK(debug_eval(system, "R[2]") == Approx(166).margin(SMALL));
	CHECK(debug_eval(system, "R[3]") == Approx(83).margin(SMALL));
	CHECK(debug_eval(system, "R[4]") == Approx(133).margin(SMALL));
	CHECK(debug_eval(system, "R[5]") == Approx(66).margin(SMALL));
	CHECK(debug_eval(system, "R[6]") == Approx(100).margin(SMALL));
	CHECK(debug_eval(system, "R[7]") == Approx(50).margin(SMALL));
	CHECK(debug_eval(system, "R[8]") == Approx(66).margin(SMALL));
	CHECK(debug_eval(system, "R[9]") == Approx(33).margin(SMALL));
	CHECK(debug_eval(system, "R[10]") == Approx(33).margin(SMALL));
	CHECK(debug_eval(system, "R[11]") == Approx(16).margin(SMALL));
	CHECK(debug_eval(system, "R[12]") == Approx(0).margin(SMALL));
	CHECK(debug_eval(system, "R[13]") == Approx(0).margin(SMALL));
}

const std::string test_prg_scale{"\
SPCLR\r\
DIM R[100]\r\
I=0\r\
SPSET 0,0,0,0,0,0\r\
\r\
SPSCALE 0,200,30\r\
@SCALE\r\
SPREAD(0),X,Y,A,S\r\
?S\r\
R[I]=S:I=I+1\r\
WAIT 5\r\
IF SPCHK(0) THEN @SCALE\r\
\r"};

TEST_CASE("Sprite scaling test", "sprites"){
	auto tokens = tokenize_str(test_prg_scale);
	
	// set tokens as program and call run_();
	PTCSystem system;
	system.get_program()->set_tokens(tokens);
	system.start();
	while (system.is_running()){
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000.0/60.0)));
		system.update();
	}
	//debug_print(system.get_visual()->c);
	
	CHECK(debug_eval(system, "R[0]") == Approx(100).margin(SMALL));
	CHECK(debug_eval(system, "R[1]") == Approx(116.65).margin(SMALL));
	CHECK(debug_eval(system, "R[2]") == Approx(133.325).margin(SMALL));
	CHECK(debug_eval(system, "R[3]") == Approx(150).margin(SMALL));
	CHECK(debug_eval(system, "R[4]") == Approx(166.65).margin(SMALL));
	CHECK(debug_eval(system, "R[5]") == Approx(183.325).margin(SMALL));
	CHECK(debug_eval(system, "R[6]") == Approx(200).margin(SMALL));
}

const std::string test_prg_angle{"\
SPCLR\r\
DIM R[100]\r\
I=0\r\
SPSET 0,0,0,0,0,0\r\
\r\
SPANGLE 0,359,30\r\
@ANGLE\r\
SPREAD(0),X,Y,A\r\
?A\r\
R[I]=A:I=I+1\r\
WAIT 5\r\
IF SPCHK(0) THEN @ANGLE\r\
\r"};

TEST_CASE("Sprite angles test", "sprites"){
	auto tokens = tokenize_str(test_prg_angle);
	
	// set tokens as program and call run_();
	PTCSystem system;
	system.get_program()->set_tokens(tokens);
	system.start();
	while (system.is_running()){
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000.0/60.0)));
		system.update();
	}
	//debug_print(system.get_visual()->c);
	
	CHECK(debug_eval(system, "R[0]") == Approx(0).margin(SMALL));
	CHECK(debug_eval(system, "R[1]") == Approx(59.832).margin(SMALL));
	CHECK(debug_eval(system, "R[2]") == Approx(119.663).margin(SMALL));
	CHECK(debug_eval(system, "R[3]") == Approx(179.5).margin(SMALL));
	CHECK(debug_eval(system, "R[4]") == Approx(239.332).margin(SMALL));
	CHECK(debug_eval(system, "R[5]") == Approx(299.163).margin(SMALL));
	CHECK(debug_eval(system, "R[6]") == Approx(359).margin(SMALL));
}

