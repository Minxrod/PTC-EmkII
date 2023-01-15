#pragma once
#include "Vars.hpp"

#include <stdexcept>

/// Specific exception class for PTC specific errors.
struct ptc_exception : public std::runtime_error {
	/// Constructor. 
	/// 
	/// @param c C string containing exception message
	ptc_exception(const char* c);
	
	/// Constructor. 
	/// 
	/// @param s std::string containing exception message
	ptc_exception(std::string c);
};

void check_command_args(std::string c, const Args& a, std::size_t l, std::size_t h);
void check_function_args(std::string c, const Vals& v, std::size_t l, std::size_t h);
