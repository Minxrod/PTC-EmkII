#include "Vars.h"
#include "Evaluator.h"
#include "BuiltinFuncs.h"
//debug
#include <iostream>

#include <regex>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include <numeric>
#include <exception>

#include <stack>

//debug
std::ostream& print(std::string name, const std::vector<Token>& items){
	std::cout << name << ":" << std::endl;
	for (auto i : items){
		if (i.text != "\r")
			std::cout << i.text << " ";
	}
	return std::cout << std::endl;
}

//debug
std::ostream& print(std::string name, const std::vector<PrioToken>& items){
	std::cout << name << ":" << std::endl;
	for (auto i : items){
		std::cout << i.text << "[" << i.prio << "] ";
	}
	return std::cout << std::endl;
}



using op_func = Var(*)(const std::vector<Var>&);
typedef std::pair<Token, op_func> op_map;
typedef op_map func_map;

const std::map<Token, op_func> operators{
	op_map{"+"_TO, ptc::add},
	op_map{"-"_TO, ptc::sub},
	op_map{"*"_TO, ptc::mul},
	op_map{"/"_TO, ptc::div},
	op_map{"%"_TO, ptc::mod},
	op_map{"!"_TO, ptc::inv},
	op_map{"=="_TO, ptc::eq},
	op_map{"!="_TO, ptc::neq},
	op_map{"<="_TO, ptc::leq},
	op_map{"<"_TO, ptc::less},
	op_map{">="_TO, ptc::geq},
	op_map{">"_TO, ptc::more},
	op_map{"OR"_TO, ptc::bor},
	op_map{"AND"_TO, ptc::band},
	op_map{"NOT"_TO, ptc::bnot},
	//op_map{"="_TO, assign},
};

const std::map<Token, op_func> functions{
	func_map{"SIN"_TF, ptc::sin},
	func_map{"COS"_TF, ptc::cos},
	func_map{"TAN"_TF, ptc::tan},
	func_map{"PI"_TF, ptc::pi},
	func_map{"ABS"_TF, ptc::abs},
	func_map{"ASC"_TF, ptc::asc},
	func_map{"ATAN"_TF, ptc::atan},
	//func_map{"CHR$"_TF, ptc::chr},
	func_map{"DEG"_TF, ptc::deg},
	func_map{"RAD"_TF, ptc::rad},
	//func_map{"EXP"_TF, ptc::exp},
	//func_map{"FLOOR"_TF, ptc::floor},
	//func_map{"HEX$"_TF, ptc::hex},
	//func_map{"LEN"_TF, ptc::len},	
	//func_map{"LEFT$"_TF, ptc::left},
	//func_map{"RIGHT$"_TF, ptc::right},
	//func_map{"MID$"_TF, ptc::mid},
	//func_map{"POW"_TF, ptc::pow},
	//func_map{"RND"_TF, ptc::rnd},
	//func_map{"SGN"_TF, ptc::sgn},
	//func_map{"SQR"_TF, ptc::sqr},
	//func_map{"STR$"_TF, ptc::str},
	//func_map{"SUBST$"_TF, ptc::subst},
	//func_map{"VAL"_TF, ptc::val},
};

VarPtr get_varptr(std::string name, std::vector<Var> args, std::map<std::string, Var>& vars){
	Var& v = vars.at(name);
	if (name.find("[]") != std::string::npos){
		if (args.size() == 2){
			auto& a_ij = std::get<Array2>(v)[std::get<Number>(args[0])][std::get<Number>(args[1])];
			if (std::holds_alternative<String>(a_ij))
				return VarPtr(&std::get<String>(a_ij));
			return VarPtr(&std::get<Number>(a_ij));
		} else if (args.size() == 1){
			auto& a_i = std::get<Array1>(v)[std::get<Number>(args[0])];
			if (std::holds_alternative<String>(a_i))
				return VarPtr(&std::get<String>(a_i));
			return VarPtr(&std::get<Number>(a_i));
		}
	}
	if (std::holds_alternative<Number>(v))
		return VarPtr(&std::get<Number>(v));
	if (std::holds_alternative<String>(v))
		return VarPtr(&std::get<String>(v));
	return VarPtr(&std::get<Array1>(v)); //cannot use 2d array refs, ever
}

Var get_var_val(std::string name, std::vector<Var> args, std::map<std::string, Var>& vars){
	//note: cannot get array by value (why would you?)
	auto ptr = get_varptr(name, args, vars);
	return (std::holds_alternative<Number*>(ptr)) ? Var(*std::get<Number*>(ptr)) : Var(*std::get<String*>(ptr));
}

void create_var(std::string name, std::vector<Var> args, std::map<std::string, Var>& vars){
	if (name.find("[]") != std::string::npos){
		if (name.find("$") != std::string::npos){
			//string type arr
			if (args.size() == 2){
				vars.insert(std::pair<std::string, Var>(name, Var(Array2{10,Array1{10,SimpleVar("")}})));
			} else {
				vars.insert(std::pair<std::string, Var>(name, Var(Array1{10,SimpleVar("")})));			
			}
		} else {
			//num type arr
			if (args.size() == 2){
				vars.insert(std::pair<std::string, Var>(name, Var(Array2{10,Array1(10,SimpleVar(0))})));
			} else {
				vars.insert(std::pair<std::string, Var>(name, Var(Array1(10,SimpleVar(0)))));		
			}
		}
	} else {
		if (name.find("$") != std::string::npos){
			//string type
			vars.insert(std::pair<std::string, Var>(name, Var(String(""))));					
		} else {
			//num type
			vars.insert(std::pair<std::string, Var>(name, Var(Number(0.0))));					
		}
	}
}

Var Evaluator::get_var(std::string name, std::vector<Var> args){
	if (vars.count(name) > 0){
		auto v = get_var_val(name, args, vars);
		return v;
	} else {
		//create new variable
		create_var(name, args, vars);
	
	}
	auto v = get_var_val(name, args, vars);
	return v;
}

VarPtr Evaluator::get_var_ptr(std::string name, std::vector<Var> args){
	if (vars.count(name) > 0){
		auto v = get_varptr(name, args, vars);
		return v;
	} else {
		//create new variable
		create_var(name, args, vars);
	}
	auto v = get_varptr(name, args, vars);
	return v;
}

Var Evaluator::evaluate(const std::vector<Token>& expression){
	//check for expression in processed;
	if (processed.count(expression) == 0){
		auto p = process(expression);
		processed.insert(std::pair<std::vector<Token>, std::vector<Token>>(expression, p));
	}
	
	auto rpn = processed.at(expression);
	
	auto res = calculate(rpn);
	return res;
}

//,
//VARS
//AND OR XOR
//== != < > <= >=
//+ -
//* / %
//! - 
//functions
//( ) [ ]
PrioToken conv_prio(const Token& t, int& n){
	PrioToken p{t.text, t.type, -1};
		
	p.prio = n; //base prio, depends on parens/nesting
	if (p.text == ","){
		p.prio += 1;
	} else if (p.text == "AND" || p.text == "OR"){
		p.prio += 2;
	} else if (p.text == "==" || p.text == "!=" || p.text == "<" ||
			p.text == ">" || p.text == "<=" || p.text == ">="){
		p.prio += 3;
	} else if (p.text == "+" || p.text == "-"){
		p.prio += 4;
	} else if (p.text == "*" || p.text == "/"){
		p.prio += 5;
	} else if (p.text == "!" || p.text == "0-" || p.text == "NOT"){
		p.prio += 6;
	} else if (p.type == Type::Func || p.type == Type::Arr){
		p.prio += 7;
	} else if (p.text == "(" || p.text == "["){
		n += 8;
		p.prio = INTERNAL_PAREN;
	} else if (p.text == ")" || p.text == "]" || p.text == "."){
		n -= 8;
		if (p.text != ".")
			p.prio = INTERNAL_PAREN;
		else
			p.prio = INTERNAL_ENDLIST;
	}
		
	return p;
}

//iterator should point to operator
std::vector<Token> op_to_rpn(std::vector<PrioToken>& e, std::vector<PrioToken>::iterator i, int n){
	auto& op = *i;
	int local_prio = op.prio % 8; 
	
	if (local_prio == 6){
		//unary op
		std::vector<Token> rpn{Token{*(i+1)}, Token{*i}};
		auto r = e.erase(i, i+2);
		e.insert(r, PrioToken{"r" + std::to_string(n), Type::Op, INTERNAL_SUBEXP});
		
		return rpn;
	} else if (local_prio == 7){
		//function/variable access
		//int nest_prio = op.prio - local_prio;
		//read until hitting lower prio (stuff not in parens)
		std::vector<Token> rpn{"."_TO};
		
		auto start = i;
		i++; //past FNC name
		while (i->prio != INTERNAL_ENDLIST){
			rpn.push_back(*i);
			i++;
		}
		rpn.push_back(*start);
		//read all args
		auto r = e.erase(start, i+1);
		e.insert(r, PrioToken{"r" + std::to_string(n), Type::Op, INTERNAL_SUBEXP});
		
		return rpn;
	} else if (local_prio == 0 && op.text != "="){
		std::vector<Token> rpn{*i};
		auto r = e.erase(i, i+1);
		e.insert(r, PrioToken{"r" + std::to_string(n), Type::Op, INTERNAL_SUBEXP});
		
		return rpn;
	} else if (local_prio == 1){
		//comma binary op
		std::vector<Token> rpn{Token{*(i-1)}, Token{*(i+1)}};
		auto r = e.erase(i-1, i+2);
		e.insert(r, PrioToken{"r" + std::to_string(n), Type::Op, INTERNAL_SUBEXP});
		
		return rpn;	
	} else {
		//binary op
		std::vector<Token> rpn{Token{*(i-1)}, Token{*(i+1)}, Token{*i}};
		auto r = e.erase(i-1, i+2);
		e.insert(r, PrioToken{"r" + std::to_string(n), Type::Op, INTERNAL_SUBEXP});
		
		return rpn;
	}
	return std::vector<Token>{};
}

std::vector<PrioToken> conv_tokens(const std::vector<Token>& expression){
	std::vector<PrioToken> tokens{};
	
	int nest = 0;
	bool is_func_paren;
	std::stack<bool> is_func{};
	for (std::vector<PrioToken>::size_type i = 0; i < expression.size(); ++i){
		Token t = expression.at(i);

		if (t.text == "("){
			is_func.push(is_func_paren);
		}
		if (t.text == ")"){
			if (is_func.top()){
				t.text = ".";
			}
			is_func.pop();
		}
		
		is_func_paren = t.type == Type::Func || t.type == Type::Arr;		
	
		if (t.text == "-"){
			if (i == 0 || expression.at(i-1).type != Type::Op){
				t.text = "0-";
			}
		} 
		
		tokens.push_back(conv_prio(t, nest));
	}

	return tokens;
}

//convert expression to RPN
std::vector<Token> Evaluator::process(const std::vector<Token>& expression){
	std::vector<PrioToken> tokens = conv_tokens(expression);
	
	//remove parenthesis
	auto end = std::remove_if(tokens.begin(), tokens.end(), [](PrioToken& p){return p.prio == INTERNAL_PAREN;});
	tokens.erase(end, tokens.end());
	
	//note: relies on internal priority "flag" values to be <0.
	int max_prio = std::max_element(tokens.begin(), tokens.end())->prio;
	
	std::vector<std::vector<Token>> subseq{};
	
	auto itr = tokens.begin();
	//print("NO_PARENS", tokens);
	
	while (max_prio >= 0){
		itr = tokens.begin();
		while (itr != tokens.end()){
			auto token = *itr;
			if (token.prio == max_prio){
				int n = subseq.size();
				auto r_n = op_to_rpn(tokens, itr, n);
				//have RPN subsequence
				subseq.push_back(r_n);
				itr = tokens.begin();
				print("TOKENS [PRIO=" +std::to_string(max_prio)+"]", tokens);			
			} else {
				itr++;
			}
		}
		max_prio--;
	}
	
	for (unsigned int i = 0; i < subseq.size(); i++){
		auto r = subseq[i];
		print("R" + std::to_string(i), r);
	}
	
	std::vector<Token> rpn{};
	auto exp = subseq.back();
	for (int i = subseq.size()-1; i >= 0; --i){
		auto sub_name = "r" + std::to_string(i);
		auto check = [&sub_name](Token& t){return (t.text == sub_name) && (t.type == Type::Op);};
		
		auto res = std::find_if(exp.begin(), exp.end(), check);
		if (res != exp.end()){
			exp.insert(res, subseq.at(i).begin(), subseq.at(i).end());
			auto end = std::remove_if(exp.begin(), exp.end(), check); 
			exp.erase(end, exp.end());
		}
	}
	
	return exp;
}

Var Evaluator::convert_to_value(const Token& t){
	if (t.type == Type::Num)
		return Var{std::stod(t.text)};
	if (t.type == Type::Str)
		return Var{t.text};
	return Var{-9876};
}

//expects a char and a \0, or expects two chars
constexpr int cstr_to_val(const char* c){
	return c[0] + (c[1] << 8);
}

Var call_op(const Token& op, std::stack<Var>& values){
//	int op_val = op.text.at(0);
//	if (op.text.size() > 1)
//		op_val += op.text.at(1) << 8;
	
	std::vector<Var> vals;
	if (op.text == "!" || op.text == "0-"){
		Var a = values.top();
		values.pop();
		
		vals.push_back(a);
	} else {
		Var a = values.top(); values.pop();
		Var b = values.top(); values.pop();
		
		vals.push_back(a);
		vals.push_back(b);
	}
	
	Var r = operators.at(op)(vals);
	
	return r;
}

Var call_func(const Token& op, std::vector<Var>& args){
	Var r = functions.at(op)(args);
	
	return r;
}

//requires expressions in the form of output from Evaluator::process()
Var Evaluator::calculate(const std::vector<Token>& rpn_expression){
	std::stack<Var> values{};
	std::stack<int> len_args{}; //for functions of unknown argument count
	bool has_varptr = false;
	VarPtr first_access{};
	
	for (auto t : rpn_expression){
		//convert to value
		if (t.type == Type::Op){
			if (t.text == "."){
				len_args.push(values.size());
			} else if (t.text == "=") {
				//assignment op is special
				if (std::holds_alternative<Number*>(first_access)){
					*std::get<Number*>(first_access) = std::get<Number>(values.top());
				} else {
					*std::get<String*>(first_access) = std::get<String>(values.top());
				}
				//don't pop value: return value assigned instead
				break;
			} else {
				//calc'd value
				auto v = call_op(t, values);
				values.push(v);
			}
		} else if (t.type == Type::Func || t.type == Type::Arr){
			//get args from stack until
			std::vector<Var> args{};
			while ((int)values.size() > len_args.top()){
				args.push_back(values.top());
				values.pop();
			}
			len_args.pop();
			//args list complete
			if (t.type == Type::Func){
				//calc'd value
				Var v = call_func(t, args);
				values.push(v);
			} else { //Type::Arr
				//ptr to array element
				Var v = get_var(t.text+"[]", args);//array access
				if (!has_varptr){
					first_access = get_var_ptr(t.text+"[]", args);
					has_varptr = true;
				}
				values.push(v);
			}
			
		} else if (t.type == Type::Var){
			//add ptr to var to stack
			Var v = get_var(t.text);
			if (!has_varptr){
				first_access = get_var_ptr(t.text);			
				has_varptr = true;
			}
			values.push(v);
		} else {
			//add value to stack
			Var v = convert_to_value(t);
			values.push(v);
		}
	}
	
	return values.top();
}
