#include "Errors.hpp"

ptc_exception::ptc_exception(const char* c) : std::runtime_error{c}{}

ptc_exception::ptc_exception(std::string s) : std::runtime_error{s.c_str()}{}
