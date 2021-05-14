#pragma once

#include "Vars.h"

#include <map>
#include <vector>
#include <mutex>
#include <string>

struct Variables {
	const std::string sysvars = " CSRX CSRY FREEMEM VERSION ERR ERL RESULT TCHX TCHY TCHST TCHTIME MAINCNTL MAINCNTH TABSTEP TRUE FALSE CANCEL ICONPUSE ICONPAGE ICONPMAX FUNCNO FREEVAR SYSBEEP KEYBOARD SPHITNO SPHITX SPHITY SPHITT TIME$ DATE$ MEM$ PRGNAME$ PACKAGE$ ";

	std::map<std::string, Var> vars;
	std::mutex sysvar_access;
	
	Variables();
	
	Var get_var(std::string name, std::vector<Var> args = {});
	VarPtr get_var_ptr(std::string name, std::vector<Var> args = {});
	void create_arr(std::string name, std::vector<Var> args = {});
	
	void clear_();
	
	void init_sysvars();
	Var get_sysvar(std::string name);

	//https://stackoverflow.com/questions/9352216/undefined-references-to-member-functions-of-a-class-template
	template <typename T>
	void write_sysvar(std::string name, T val){
		std::lock_guard g(sysvar_access);
		*std::get<T*>(get_var_ptr(name)) = val;
	}
};


