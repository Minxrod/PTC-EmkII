#include "Variables.h"

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

void create_var(std::string name, std::map<std::string, Var>& vars, std::vector<Var> args = {}){
	std::size_t dimen1 = 10;
	std::size_t dimen2 = -1;
	if (args.size() > 0)
		dimen1 = static_cast<int>(std::get<Number>(args[0]));
	if (args.size() > 1)
		dimen2 = static_cast<int>(std::get<Number>(args[1]));
	
	if (name.find("[]") != std::string::npos){
		if (name.find("$") != std::string::npos){
			if (args.size() == 2){
				vars.insert(std::pair<std::string, Var>(name, Var(Array2(dimen1,Array1(dimen2,SimpleVar(""))))));
			} else {
				vars.insert(std::pair<std::string, Var>(name, Var(Array1(dimen1,SimpleVar("")))));			
			}
		} else {
			if (args.size() == 2){
				vars.insert(std::pair<std::string, Var>(name, Var(Array2(dimen1,Array1(dimen2,SimpleVar(0))))));
			} else {
				vars.insert(std::pair<std::string, Var>(name, Var(Array1(dimen1,SimpleVar(0)))));		
			}
		}
	} else {
		if (name.find("$") != std::string::npos){
			vars.insert(std::pair<std::string, Var>(name, Var(String(""))));					
		} else {
			vars.insert(std::pair<std::string, Var>(name, Var(Number(0.0))));					
		}
	}
}

Var Variables::get_var(std::string name, std::vector<Var> args){
	if (vars.count(name) > 0){
		auto v = get_var_val(name, args, vars);
		return v;
	} else {
		//create new variable
		create_var(name, vars);
	
	}
	auto v = get_var_val(name, args, vars);
	return v;
}

VarPtr Variables::get_var_ptr(std::string name, std::vector<Var> args){
	if (vars.count(name) > 0){
		auto v = get_varptr(name, args, vars);
		return v;
	} else {
		//create new variable
		create_var(name, vars);
	}
	auto v = get_varptr(name, args, vars);
	return v;
}

void Variables::create_arr(std::string name, std::vector<Var> args){
	create_var(name, vars, args);
}

