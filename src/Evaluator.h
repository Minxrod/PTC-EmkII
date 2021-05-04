#pragma once

#include "Vars.h"

#include <stack>
#include <string>
#include <vector>
#include <map>

const int INTERNAL_ENDLIST = -999;
const int INTERNAL_SUBEXP = -888;
const int INTERNAL_PAREN = -24;

std::vector<PrioToken> conv_tokens(const std::vector<Token>&);

struct Evaluator {
	std::map<Token, op_func> operators;
	std::map<Token, op_func> functions;

	std::map<std::string, Var> vars;
	std::map<std::vector<Token>, std::vector<Token>> processed; 

	Evaluator();
	void add_funcs(std::map<Token, op_func>);
	
	Var get_var(std::string name, std::vector<Var> args = {});
	VarPtr get_var_ptr(std::string name, std::vector<Var> args = {});
	//VarPtr get_var_ptr(std::string name);

	Var convert_to_value(const Token&);
	Var call_op(const Token&, std::stack<Var>&);
	Var call_func(const Token&, std::vector<Var>&);
	
	//Takes an expression and evaluates it to a value
	Var evaluate(const std::vector<Token>&);
	
	//takes an expression and modifies it to a calculatable form
	std::vector<Token> process(const std::vector<Token>&);
	
	//takes a command and breaks it into smaller arg/cmd chunks
	//ex. PRINT 5,6,7;8+9,ARR[I)
	//std::vector<std::vector<Token>> split(const std::vector<Token>&);
	
	//takes a calculable form and gets a value
	Var calculate(const std::vector<Token>&);
};
