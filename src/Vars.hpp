#pragma once

#include <string>
#include <variant>
#include <vector>
#include <utility>
#include <functional>
#include <cstddef>

#include "Tokens.hpp"

typedef double Number;
// when you realize that BGF1 exists and is usable
// note: assumes std::wchar_t is at least 16 bits
typedef std::wstring String;

std::string to_string(const String&);

// because I do not feel like rewriting a bunch of literal comparisons
bool operator==(const String&, std::string&);
bool operator==(const String&, const char*);
bool operator!=(const String&, const char*);

using SimpleVar = std::variant<Number, String>;

typedef std::vector<SimpleVar> Array1;
typedef std::vector<Array1> Array2;

using Var = std::variant<Number, String, Array1, Array2>;
using VarPtr = std::variant<Number*, String*, Array1*>;

typedef const std::vector<Var>& Vals;
typedef std::vector<Token> Expr;
typedef const std::vector<Expr>& Args;

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

