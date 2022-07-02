#include "Tokens.hpp"

bool operator<(const Token& a, const Token& b){
	return a.text < b.text ? true : b.text < a.text ? false : a.type < b.type;
}

bool operator<(const PrioToken& a, const PrioToken& b){
	return a.prio < b.prio;
}

bool operator==(const Token& a, const Token& b){
	return a.type == b.type && a.text == b.text;
}

Token operator""_TO(const char* x, std::size_t y){
	return Token{std::string{x, y}, Type::Op};
}

Token operator""_TF(const char* x, std::size_t y){
	return Token{std::string{x, y}, Type::Func};
}

Token operator""_TC(const char* x, std::size_t y){
	return Token{std::string{x, y}, Type::Cmd};
}

std::ostream& print(std::string name, const std::vector<Token>& items){
	std::cout << name << ":" << std::endl;
	for (auto i : items){
		if (i.text != "\r")
			std::cout << i.text << " ";
	}
	return std::cout << std::endl;
}

std::ostream& print(std::string name, const std::vector<PrioToken>& items){
	std::cout << name << ":" << std::endl;
	for (auto i : items){
		std::cout << i.text << "[" << i.prio << "] ";
	}
	return std::cout << std::endl;
}
