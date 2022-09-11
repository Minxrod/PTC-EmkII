#include "test_util.hpp"

#include "PTCSystem.hpp"

#include "Tokens.hpp"

Expr tokenize_str(std::string expr){
	return tokenize(reinterpret_cast<unsigned char*>(const_cast<char*>(expr.c_str())), expr.size());
}


void debug_print(const PTC2Console& con){
	for (int y = 0; y < 24; ++y){
		for (int x = 0; x < 32; ++x){
			char c = con.chkchr(x,y);
			std::cout << (c ? c : ' ');
		}
		std::cout << std::endl;
	}
}

Number debug_eval(PTCSystem& system, const std::string expr){
	return std::get<Number>(system.get_evaluator()->evaluate(tokenize_str(expr)));
}
