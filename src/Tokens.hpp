#pragma once

#include <string>
#include <vector>
#include <cstddef>

/// Tokenization types (What a given string will be used for/is)
enum class Type { Num, Str, Arr, Var, Op, Func, Cmd, Newl, Rem, Label, Internal};

/// Token struct.
/// 
/// Used when tokenizing programs, and for evaluating expressions.
struct Token {
	/// String for this token
	std::string text;
	/// Type of token
	Type type;
};

/// Priority token.
/// 
/// Used to convert expressions into RPN.
struct PrioToken : public Token{
	/// Priority of this token.
	int prio;
};

typedef std::vector<Token> Expr;
typedef const std::vector<Expr>& Args;

bool operator<(const Token& a, const Token& b);
bool operator<(const PrioToken& a, const PrioToken& b);
bool operator==(const Token& a, const Token& b);
Token operator""_TO(const char*, std::size_t);
Token operator""_TF(const char*, std::size_t);
Token operator""_TC(const char*, std::size_t);

//debug
#include <iostream>
std::ostream& print(std::string name, const std::vector<Token>& items);
std::ostream& print(std::string name, const std::vector<PrioToken>& items);
