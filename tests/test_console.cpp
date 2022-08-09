#include "catch.hpp"
#include "test_util.hpp"

#include <thread>
#include <iostream>

#include "PTCSystem.hpp"

const std::string test_prg = "CLS\r\
PRINT \"HELLO\"\r\
PRINT 87.5\r\
A$=\"36.7\"\r\
B=&HFF\r\
PRINT A$\r\
PRINT B\r\
LOCATE 3,6\r\
PRINT \"ABCDE\"\r\
LOCATE 24,9\r\
PRINT \"12345678\",0\r\
LOCATE 24,10\r\
PRINT \"12345678\";0\r\
PRINT 36;,27,;48,59;63\r\
LOCATE 24,14\r\
PRINT \"1234567\",8\r\
LOCATE 0,23\r\
PRINT \"23\"*16;\r\
LOCATE 10,21\r";

TEST_CASE("Console test", "console"){
	auto tokens = tokenize_str(test_prg);
	
	// set tokens as program and call run_();
	PTCSystem system;
	system.get_program()->set_tokens(tokens);
	system.start();
	while (system.is_running()){
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000.0/60.0)));
		system.update();
	}
	
	auto& con = system.get_visual()->c;
	auto check_str = [&con](int x, int y, const char* s){
		int i = 0;
		while (s[i] != '\0'){
			CHECK((char)con.chkchr(x,y) == s[i]);
			++x;
			++i;
		}
	};
	
	check_str(0,0,"HELLO");
	check_str(0,1,"87.5");
	check_str(0,2,"36.7");	
	check_str(0,3,"255");
	
	check_str(3,6,"ABCDE");
	
	check_str(24,9,"12345678");
	CHECK((char)con.chkchr(4,10) == '0');
	check_str(24,10,"12345678");
	CHECK((char)con.chkchr(0,11) == '0');
	
	CHECK((char)con.chkchr(0,12) == '3');
	CHECK((char)con.chkchr(1,12) == '6');
	CHECK((char)con.chkchr(4,12) == '2');
	CHECK((char)con.chkchr(5,12) == '7');
	CHECK((char)con.chkchr(8,12) == '4');
	CHECK((char)con.chkchr(9,12) == '8');
	CHECK((char)con.chkchr(12,12) == '5');
	CHECK((char)con.chkchr(13,12) == '9');
	CHECK((char)con.chkchr(14,12) == '6');
	CHECK((char)con.chkchr(15,12) == '3');
	
	check_str(24,14,"1234567");
	CHECK((char)con.chkchr(0,15) == '8');
	
	check_str(0,23,"23232323232323232323232323232323");
	
	//display for debug
	for (int y = 0; y < 24; ++y){
		for (int x = 0; x < 32; ++x){
			char c = con.chkchr(x,y);
			std::cout << (c ? c : ' ');
		}
		std::cout << std::endl;
	}
	
}
