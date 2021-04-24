#include "Vars.h"

bool operator<(const Token& a, const Token& b){
	return a.text < b.text ? true : b.text < a.text ? false : a.type < b.type;
}

bool operator<(const PrioToken& a, const PrioToken& b){
	return a.prio < b.prio;
}

bool operator==(const Token& a, const Token& b){
	return a.type == b.type && a.text == b.text;
}

Token operator""_TO(const char* x, long unsigned int y){
	return Token{std::string{x, y}, Type::Op};
}

Token operator""_TF(const char* x, long unsigned int y){
	return Token{std::string{x, y}, Type::Func};
}

Token operator""_TC(const char* x, long unsigned int y){
	return Token{std::string{x, y}, Type::Cmd};
}
