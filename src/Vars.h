#pragma once

#include <string>
#include <variant>
#include <vector>
#include <regex>

typedef double Number;
typedef std::string String;

using SimpleVar = std::variant<Number, String>;

typedef std::vector<SimpleVar> Array1;
typedef std::vector<Array1> Array2;

using Var = std::variant<Number, String, Array1, Array2>;
using VarPtr = std::variant<Number*, String*, Array1*>;

enum class Type { Num, Str, Arr, Var, Op, Func, Cmd, Newl, Rem, Label, Internal};

struct Token {
	std::string text;
	Type type;
};

struct PrioToken : public Token{
	int prio;
};

typedef std::vector<Token> Expr;
typedef const std::vector<Expr>& Args;
using cmd_type = std::function<void(const Args&)>;
typedef std::pair<Token, cmd_type> cmd_map;

bool operator<(const Token& a, const Token& b);
bool operator<(const PrioToken& a, const PrioToken& b);
bool operator==(const Token& a, const Token& b);
Token operator""_TO(const char*, long unsigned int);
Token operator""_TF(const char*, long unsigned int);
Token operator""_TC(const char*, long unsigned int);

template <typename T>
auto getfunc(T* obj, void(T::* fptr)(const Args&)){
	return [obj, fptr](const Args& a) -> void{
		(obj->*fptr)(a);
	};
}


