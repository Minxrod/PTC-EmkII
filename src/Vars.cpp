#include "Vars.hpp"

bool operator==(const String& s1, std::string& s2){
	if (s1.length() != s2.length())
		return false;
	for (std::size_t i = 0; i < s1.length(); ++i){
		if (s1[i] != s2[i]) return false;
	}
	return true;
}

bool operator==(const String& s1, const char* s2){
	for (std::size_t i = 0; i < s1.length(); ++i){
		if (s2[i] == '\0') return false; // string not long enough
		if (s1[i] != s2[i]) return false;
	}
	return true;
}

bool operator!=(const String& s1, const char* s2){
	return !(s1 == s2);
}

std::string to_string(const String& s){
	return std::string{s.cbegin(), s.cend()};
}


