#pragma once

#include <stdexcept>

/// Specific exception class for PTC specific errors.
struct ptc_exception : public std::runtime_error {
	/// Constructor. 
	/// 
	/// @param c C string containing exception message
	ptc_exception(const char* c);
};

#define PTC_COMMAND_ARGS(c, a, l, h) if (a.size() < l || a.size() > h) throw ptc_exception(a.size() <= h ? "Missing operand (##c)" : "Syntax error (##c)");
#define PTC_FUNCTION_ARGS(c, v, l, h) if (v.size() < l || v.size() > h) throw ptc_exception(v.size() > h ? "Missing operand (##c)" : "Syntax error (##c)");

#define PTC_MISSING_OPERAND(c, n) if (a.size() == n) { throw ptc_exception{"Missing operand (##c)"}; }

