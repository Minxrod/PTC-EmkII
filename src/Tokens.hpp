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

/// Tokenizes the given data as a PTC2 program.
/// 
/// @note This was some of the first code created for this iteration of the project, 
/// and could probably be more efficient.
/// 
/// @param data Pointer to PRG data to tokenize
/// @param size Length of data pointed to
/// @return Vector of Tokens
std::vector<Token> tokenize(unsigned char* data, std::size_t size);

//debug
#include <iostream>
std::ostream& print(std::string name, const std::vector<Token>& items);
std::ostream& print(std::string name, const std::vector<PrioToken>& items);
