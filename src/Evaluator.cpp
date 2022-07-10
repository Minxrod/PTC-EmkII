#include "Vars.hpp"
#include "Evaluator.hpp"
#include "BuiltinFuncs.hpp"
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

Evaluator::Evaluator(){
	operators = std::map<Token, op_func>{
		op_map{"+"_TO, ptc::add},
		op_map{"-"_TO, ptc::sub},
		op_map{"0-"_TO, ptc::neg},
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
		op_map{"XOR"_TO, ptc::bxor},
	};
	
	functions = get_funcs();
}

std::map<Token, cmd_type> Evaluator::get_cmds() {
	return std::map<Token, cmd_type>{
		cmd_map("DTREAD"_TC, getfunc(this, &Evaluator::dtread_)),
		cmd_map("TMREAD"_TC, getfunc(this, &Evaluator::tmread_))
	};
}
	
std::map<Token, op_func> Evaluator::get_funcs() {
	return std::map<Token, op_func>{
		func_map{"SIN"_TF, ptc::sin},
		func_map{"COS"_TF, ptc::cos},
		func_map{"TAN"_TF, ptc::tan},
		func_map{"PI"_TF, ptc::pi},
		func_map{"ABS"_TF, ptc::abs},
		func_map{"ASC"_TF, ptc::asc},
		func_map{"ATAN"_TF, ptc::atan},
		func_map{"CHR$"_TF, ptc::chr},
		func_map{"DEG"_TF, ptc::deg},
		func_map{"RAD"_TF, ptc::rad},
		//func_map{"EXP"_TF, ptc::exp},
		func_map{"FLOOR"_TF, ptc::floor},
		func_map{"HEX$"_TF, ptc::hex},
		func_map{"LEN"_TF, ptc::len},	
		func_map{"LEFT$"_TF, ptc::left},
		func_map{"RIGHT$"_TF, ptc::right},
		func_map{"MID$"_TF, ptc::mid},
		func_map{"POW"_TF, ptc::pow},
		func_map{"RND"_TF, ptc::rnd},
		func_map{"SGN"_TF, ptc::sgn},
		func_map{"SQR"_TF, ptc::sqr},
		func_map{"STR$"_TF, ptc::str},
		func_map{"SUBST$"_TF, ptc::subst},
		func_map{"VAL"_TF, ptc::val},
		func_map{"INSTR"_TF, ptc::instr},
	};
}

void Evaluator::add_funcs(std::map<Token, op_func> other){
	functions.merge(other);
}

void Evaluator::dtread_(const Args& a){
	//DTREAD (yyyy/mm/dd) y m d 
	String str = std::get<String>(evaluate(a[1]));
	
	assign(a[2], Token{str.substr(0,4), Type::Str});
	assign(a[3], Token{str.substr(5,2), Type::Str});
	assign(a[4], Token{str.substr(8,2), Type::Str});
}


void Evaluator::tmread_(const Args& a){
	//TMREAD hh:mm:ss h m s
	String str = std::get<String>(evaluate(a[1]));

	assign(a[2], Token{str.substr(0,2), Type::Str});
	assign(a[3], Token{str.substr(3,2), Type::Str});
	assign(a[4], Token{str.substr(6,2), Type::Str});
}

Var Evaluator::evaluate(const std::vector<Token>& expression){
	//check for expression in processed;
	if (processed.count(expression) == 0){
		auto p = process(expression);
		processed.insert(std::pair<std::vector<Token>, std::vector<Token>>(expression, p));
	}
	
	auto rpn = processed.at(expression);
	
	auto res = calculate(rpn);
	return res.back();
}

//Version of evaluate that returns a list of results
std::vector<Var> Evaluator::eval_all_results(const std::vector<Token>& expression){
	if (processed.count(expression) == 0){
		auto p = process(expression);
		processed.insert(std::pair<std::vector<Token>, std::vector<Token>>(expression, p));
	}
	
	auto rpn = processed.at(expression);
	
	return calculate(rpn);
}

Var Evaluator::eval_no_save(const std::vector<Token>& expression){
	//no check for expression in processed, no store processed expression.
	auto rpn = process(expression);
	auto res = calculate(rpn);
	return res.back();
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
	if (p.type == Type::Op || p.type == Type::Arr || p.type == Type::Func){ //if it's not an op, don't change prio
		if (p.text == ","){
			p.prio += 1;
		} else if (p.text == "AND" || p.text == "OR" || p.text == "XOR"){
			p.prio += 2;
		} else if (p.text == "==" || p.text == "!=" || p.text == "<" ||
				p.text == ">" || p.text == "<=" || p.text == ">="){
			p.prio += 3;
		} else if (p.text == "+" || p.text == "-"){
			p.prio += 4;
		} else if (p.text == "*" || p.text == "/" || p.text == "%"){
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
	}
	if (p.type == Type::Rem){
		p.prio = COMMENT;
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
	} else if (local_prio == 0 && !(op == "="_TO)){
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
	bool is_func_paren = false;
	std::stack<bool> is_func{};
	for (std::vector<PrioToken>::size_type i = 0; i < expression.size(); ++i){
		Token t = expression.at(i);

		if (t.text == "("){
			is_func.push(is_func_paren);
		}
		if (t.text == ")" && t.type != Type::Str){
			if (is_func.top()){
				t.text = ".";
			}
			is_func.pop();
		}
		
		is_func_paren = t.type == Type::Func || t.type == Type::Arr;		
	
		if (t == "-"_TO){
			if (i == 0 || (expression.at(i-1).type == Type::Op && expression.at(i-1).text != ")")){
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
//	print("NO_PARENS", tokens);
	
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
//				print("TOKENS [PRIO=" +std::to_string(max_prio)+"]", tokens);
			} else {
				itr++;
			}
		}
		max_prio--;
	}
	
	for (unsigned int i = 0; i < subseq.size(); i++){
		auto r = subseq[i];
//		print("R" + std::to_string(i), r);
	}
	
	auto exp = std::vector<Token>{};
	for (auto& t : tokens){
		exp.push_back(Token{t.text, t.type});
	}
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

Var convert_to_value(const Token& t){
	if (t.type == Type::Num){
		if (t.text.size() >= 2){
			if (t.text[1] == 'H')
				return Var((double)std::stoi(t.text.substr(2), nullptr, 16));
			else if (t.text[1] == 'B')
				return Var((double)std::stoi(t.text.substr(2), nullptr, 2));
			}
		return Var{std::stod(t.text)};
	}
	if (t.type == Type::Str)
		return Var{t.text};
	if (t.type == Type::Label)
		return Var{t.text};
	throw std::logic_error{"error: convert_to_value failure"};
}

void assign_varptr(VarPtr vp, Var v){
	if (std::holds_alternative<Number*>(vp)){
		*std::get<Number*>(vp) = std::get<Number>(v);
	} else {
		*std::get<String*>(vp) = std::get<String>(v);
	}
}

void Evaluator::assign(const Expr& exp, Token t){
	auto p = process(exp);
	auto name = exp[0];
	
	if (name.type == Type::Arr){
		name.text += "[]";
	}
	
	if (name.text.find("$") != std::string::npos){
		t.type = Type::Str;
	} else {
		t.type = Type::Num;
	}
	
	//RPN form of <var>=<value> is <var>,<value>,=
	p.push_back(t);
	p.push_back("="_TO);
	
	calculate(p);
}

Var Evaluator::call_op(const Token& op, std::stack<Var>& values){
//	int op_val = op.text.at(0);
//	if (op.text.size() > 1)
//		op_val += op.text.at(1) << 8;
	
	std::vector<Var> vals;
	if (op.text == "!" || op.text == "0-" || op.text == "NOT"){
		Var a = values.top();
		values.pop();
		
		vals.push_back(a);
	} else {
		Var a = values.top(); values.pop();
		Var b = values.top(); values.pop();
		
		vals.push_back(b);
		vals.push_back(a);
	}
	
	Var r = operators.at(op)(vals);
	
	return r;
}

Var Evaluator::call_func(const Token& f, std::vector<Var>& args){
	Var r = functions.at(f)(args);
	
	return r;
}

//requires expressions in the form of output from Evaluator::process()
std::vector<Var> Evaluator::calculate(const std::vector<Token>& rpn_expression, bool do_array_init){
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
				assign_varptr(first_access, values.top());
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
			std::reverse(args.begin(), args.end()); //stack has reversed order of args
			//args list complete
			if (t.type == Type::Func){
				//calc'd value
				Var v = call_func(t, args);
				values.push(v);
			} else { //Type::Arr
				if (do_array_init && len_args.empty()){//special array creation hack
					//needs to handle needing args for array creation otherwise
					//might have array ACCESS as part of args, so must have len_args be empty.
					vars.create_arr(t.text+"[]", args);
					break;
				} else {
					//ptr to array element
					Var v = vars.get_var(t.text+"[]", args);//array access
					if (!has_varptr && len_args.empty()){
						first_access = vars.get_var_ptr(t.text+"[]", args);
						has_varptr = true;
					}
					values.push(v);
				}
			}
			
		} else if (t.type == Type::Var){
			//add ptr to var to stack
			Var v = vars.get_var(t.text);
			if (!has_varptr && len_args.empty()){
				first_access = vars.get_var_ptr(t.text);			
				has_varptr = true;
			}
			values.push(v);
		} else if (t.type == Type::Rem){
			// do nothing, because this is a comment
		} else {
			//add value to stack
			Var v = convert_to_value(t);
			values.push(v);
		}
	}
	
	std::vector<Var> results;
	while (values.size()){
		results.push_back(values.top());
		values.pop();
	}
	
	return results;
}

//general tokenization stuff here

std::vector<std::vector<Token>> split(const std::vector<Token>& expression){
	std::vector<std::vector<Token>> subexp{};
	
	std::vector<PrioToken> all = conv_tokens(expression);
	
	auto i = 0;
	auto start = expression.begin();
	auto old = i;
	while (i < (int)all.size()){
		if (all.at(i).type == Type::Cmd){
			if (old != i) //ON ... GOTO IF ... THEN 
				subexp.push_back(std::vector<Token>(start+old, start+i));
			subexp.push_back(std::vector<Token>{all.at(i)}); //some command
			old=i+1;
		}
		if (all.at(i).prio == 1){
			//found low-prio comma
			subexp.push_back(std::vector<Token>(start+old, start+i));
			old=i+1;
		}
		i++;
	}
	//this works since there should always be at least one subexp.
	if (i != old)
		subexp.push_back(std::vector<Token>(start+old,start+i));
		
	return subexp;
}


