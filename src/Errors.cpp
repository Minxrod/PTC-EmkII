#include "Errors.hpp"

ptc_exception::ptc_exception(const char* c) : std::runtime_error{c}{}

ptc_exception::ptc_exception(std::string s) : std::runtime_error{s.c_str()}{}

void check_command_args(std::string c, const Args& a, std::size_t l, std::size_t h){
	if (a.size() < l || a.size() > h)
		throw ptc_exception(a.size() <= h ? "Missing operand ("+c+")" : "Syntax error ("+c+")");
}

void check_function_args(std::string c, const Vals& v, std::size_t l, std::size_t h){
	if (v.size() < l || v.size() > h)
		throw ptc_exception(v.size() > h ? "Missing operand ("+c+")" : "Syntax error ("+c+")");
}

