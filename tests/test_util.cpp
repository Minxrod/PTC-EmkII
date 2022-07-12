#include "Tokens.hpp"

Expr tokenize_str(std::string expr){
	return tokenize(reinterpret_cast<unsigned char*>(const_cast<char*>(expr.c_str())), expr.size());
}
