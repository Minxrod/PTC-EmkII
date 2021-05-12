#pragma once

#include "Vars.h"

#include <map>

struct Variables {
	std::map<std::string, Var> vars;
	
	Var get_var(std::string name, std::vector<Var> args = {});
	VarPtr get_var_ptr(std::string name, std::vector<Var> args = {});
	void create_arr(std::string name, std::vector<Var> args = {});
};

