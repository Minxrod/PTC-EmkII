#pragma once

#include <string>
#include <variant>
#include <vector>
#include <regex>

typedef double Number;
typedef std::string String;
typedef std::vector<double> Num1d;
typedef std::vector<std::string> Str1d;
typedef std::vector<std::vector<double>> Num2d;
typedef std::vector<std::vector<std::string>> Str2d;
typedef int Internal;

using Var = std::variant<Number, String, Num1d, Str1d, Num2d, Str2d>;

enum class Type { Num, Str, Arr, Var, Op, Func, Cmd, Newl, Rem, Label, Internal};

struct Token {
	std::string text;
	Type type;
};

struct PrioToken : public Token{
	int prio;
};

bool operator<(const Token& a, const Token& b);
bool operator<(const PrioToken& a, const PrioToken& b);
bool operator==(const Token& a, const Token& b);
Token operator""_TO(const char*, long unsigned int);
Token operator""_TF(const char*, long unsigned int);


