#pragma once

#include "Vars.h"

#include <stack>
#include <string>
#include <vector>
#include <map>

std::ostream& print(std::string name, const std::vector<Token>&);
std::ostream& print(std::string name, const std::vector<PrioToken>&);

const int INTERNAL_ENDLIST = -999;
const int INTERNAL_SUBEXP = -888;
const int INTERNAL_PAREN = -23;

struct Evaluator {
	std::map<std::string, Var> vars;
	std::map<std::vector<Token>, std::vector<Token>> processed; 
	
	Var get_var(std::string name, std::vector<Var> args = {});

	Var* get_var_ptr(std::string name);

	Var convert_to_value(const Token&);
	
	//Takes an expression and evaluates it to a value
	Var evaluate(const std::vector<Token>&);
	
	//takes an expression and modifies it to a calculatable form
	std::vector<Token> process(const std::vector<Token>&);
	
	//takes a command and breaks it into calculable forms
	//ex. PRINT 5,6,7;8+9,ARR[I)
	std::vector<std::vector<Token>> split(const std::vector<Token>&);
	
	//takes a calculable form and gets a value
	Var calculate(const std::vector<Token>&);
};
