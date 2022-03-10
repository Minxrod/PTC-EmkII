#pragma once

#include <string>
#include <variant>
#include <vector>
#include <utility>
#include <functional>

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
typedef const std::vector<Var>& Vals;

using cmd_type = std::function<void(const Args&)>;
typedef std::pair<Token, cmd_type> cmd_map;

using op_func = std::function<Var(const Vals&)>;
typedef std::pair<Token, op_func> func_map;
typedef func_map op_map;

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

template <typename T>
auto getfunc(T* obj, Var(T::* fptr)(const Vals&)){
	return [obj, fptr](const Vals& v) -> Var{
		return (obj->*fptr)(v);
	};
}

//debug
#include <iostream>
std::ostream& print(std::string name, const std::vector<Token>& items);
std::ostream& print(std::string name, const std::vector<PrioToken>& items);
