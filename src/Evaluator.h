#pragma once

#include "Vars.h"
#include "Variables.h"

#include <stack>
#include <string>
#include <vector>
#include <map>

//values don't really matter, just need to be negative and unique
const int INTERNAL_ENDLIST = -999;
const int INTERNAL_SUBEXP = -888;
const int INTERNAL_PAREN = -24;

std::vector<PrioToken> conv_tokens(const std::vector<Token>&);
Var convert_to_value(const Token& t);
std::vector<Token> tokenize(unsigned char*, int);
std::vector<std::vector<Token>> split(const std::vector<Token>&);

struct Evaluator {
	Variables vars;
	std::map<Token, op_func> operators;
	std::map<Token, op_func> functions;
	//expressions that have been processed once already can be saved
	std::map<std::vector<Token>, std::vector<Token>> processed; 

	Evaluator();
	void add_funcs(std::map<Token, op_func>);
		
	void assign(const Expr& exp, Token t);
	
	Var call_op(const Token&, std::stack<Var>&);
	Var call_func(const Token&, std::vector<Var>&);
	
	//Takes an expression and evaluates it to a value
	Var evaluate(const std::vector<Token>&);
	Var eval_no_save(const std::vector<Token>&);
	
	//takes an expression and modifies it to a calculatable form
	std::vector<Token> process(const std::vector<Token>&);
	
	//takes a calculable form and gets a value
	Var calculate(const std::vector<Token>&, bool = false);
	
	void dtread_(const Args&);
	void tmread_(const Args&);
};
