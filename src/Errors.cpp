#include "Errors.hpp"

ptc_exception::ptc_exception(const char* c) : std::runtime_error{c}{}
