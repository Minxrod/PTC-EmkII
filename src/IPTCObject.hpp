#pragma once

#include <functional>
#include <map>

#include "Vars.hpp"

using cmd_type = std::function<void(const Args&)>;
typedef std::pair<Token, cmd_type> cmd_map;

using op_func = std::function<Var(const Vals&)>;
typedef std::pair<Token, op_func> func_map;
typedef func_map op_map;

/// Interface allowing objects to provide access to PTC commands/functions.
/// 
/// Expected function signature is `Var func(const Vals&)`
/// 
/// Expected command signature is `void cmd(const Args&)`
class IPTCObject {
public:
	/// Creates a map of PTC command tokens to callable commands.
	/// 
	/// @return Map of Tokens to PTC commands
	virtual std::map<Token, cmd_type> get_cmds() = 0;
	
	/// Creates and returns a map of PTC function tokens to callable functions.
	/// 
	/// @return Map of Tokens to PTC functions
	virtual std::map<Token, op_func> get_funcs() = 0;
	
	/// Virtual destructor
	virtual ~IPTCObject() {}
};
