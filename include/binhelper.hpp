#pragma once

#include <string>

int binary_as_int(char* data);

short binary_as_short(char* data);

unsigned short binary_as_ushort(char* data);

unsigned short binary_as_u24(char* data);

std::string as_hex(int i);
