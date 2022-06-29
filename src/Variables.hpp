#pragma once

#include "Vars.hpp"

#include <map>
#include <vector>
#include <mutex>
#include <string>

/// PTC variable management struct. Manages creation and deletion of variables.
struct Variables {
	/// List of all valid system variables, separated by spaces.
	const std::string sysvars = " CSRX CSRY FREEMEM VERSION ERR ERL RESULT TCHX TCHY TCHST TCHTIME MAINCNTL MAINCNTH TABSTEP TRUE FALSE CANCEL ICONPUSE ICONPAGE ICONPMAX FUNCNO FREEVAR SYSBEEP KEYBOARD SPHITNO SPHITX SPHITY SPHITT TIME$ DATE$ MEM$ PRGNAME$ PACKAGE$ ";
	
	/// Map of variable names to variables.
	std::map<std::string, Var> vars;
	/// Mutex for writing to system variables across multiple threads.
	std::mutex sysvar_access;
	
	/// Default constructor
	Variables();
	
	/// Gets the value of a variable.
	/// 
	/// @param name Name of variable
	/// @param args Indexes for arrays, if applicable
	/// @return Value of variable
	Var get_var(std::string name, std::vector<Var> args = {});
	/// Gets a pointer to the value of a variable.
	/// 
	/// @param name Name of variable
	/// @param args Indexes for arrays, if applicable
	/// @return Pointer to variable
	VarPtr get_var_ptr(std::string name, std::vector<Var> args = {});
	/// Creates an array.
	/// 
	/// @param name Name of variable
	/// @param args Size of array
	void create_arr(std::string name, std::vector<Var> args = {});
	
	/// Clears all variables except system variables.
	void clear_();
	
	/// Initializes the values of several constant system variables.
	void init_sysvars();
	/// Returns the value of a system variables.
	/// 
	/// @param name Name of system variable
	/// @return Value of system variable
	Var get_sysvar(std::string name);

	//https://stackoverflow.com/questions/9352216/undefined-references-to-member-functions-of-a-class-template
	/// Writes to a system variable.
	/// 
	/// @param name Name of variable
	/// @param val Value to write to variable
	template <typename T>
	void write_sysvar(std::string name, T val){
		std::lock_guard g(sysvar_access);
		*std::get<T*>(get_var_ptr(name)) = val;
	}
};


