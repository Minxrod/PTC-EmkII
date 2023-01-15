#include "Variables.hpp"
#include "Errors.hpp"

Variables::Variables(){
	init_sysvars();
}

VarPtr get_varptr(std::string name, std::vector<Var> args, std::map<std::string, Var>& vars){
	Var& v = vars.at(name);
	if (name.find("[]") != std::string::npos){
		if (args.size() == 2){
			Number i = std::get<Number>(args[0]);
			Number j = std::get<Number>(args[1]);
			auto& arr = std::get<Array2>(v);
			if (arr.size()<=i || i<0 || arr[i].size()<=j || j<0){
				throw ptc_exception{"Subscript out of range"};
			}
			
			auto& a_ij = arr[i][j];
			if (std::holds_alternative<String>(a_ij))
				return VarPtr(&std::get<String>(a_ij));
			return VarPtr(&std::get<Number>(a_ij));
		} else if (args.size() == 1){
			Number i = std::get<Number>(args[0]);
			auto& arr = std::get<Array1>(v);
			if (arr.size()<=i || i<0){
				throw ptc_exception{"Subscript out of range"};
			}
			auto& a_i = arr[i];
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
				vars.insert(std::pair<std::string, Var>(name, Var(Array2(dimen1,Array1(dimen2,SimpleVar(String()))))));
			} else {
				vars.insert(std::pair<std::string, Var>(name, Var(Array1(dimen1,SimpleVar(String())))));			
			}
		} else {
			if (args.size() == 2){
				vars.insert(std::pair<std::string, Var>(name, Var(Array2(dimen1,Array1(dimen2,SimpleVar(0.0))))));
			} else {
				vars.insert(std::pair<std::string, Var>(name, Var(Array1(dimen1,SimpleVar(0.0)))));
			}
		}
	} else {
		if (name.find("$") != std::string::npos){
			vars.insert(std::pair<std::string, Var>(name, Var(String())));					
		} else {
			vars.insert(std::pair<std::string, Var>(name, Var(Number(0.0))));					
		}
	}
}

//This gets variables by VALUE, including sysvars.
Var Variables::get_var(std::string name, std::vector<Var> args){
	if (sysvars.find(" "+name+" ") != std::string::npos)
		return get_sysvar(name);
	
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
	if (sysvars.find(" "+name+" ") != std::string::npos){
		//should not happen in most cases...
	}
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

//Vars to worry about threading with: 
//Constant: VERSION TRUE FALSE CANCEL
//Write:    SYSBEEP ICONPUSE ICONPAGE ICONPMAX MEM$ TABSTEP
//Safe:     CSRX CSRY FREEMEM FREEVAR RESULT SPHITNO SPHITX SPHITY PRGNAME$ PACKAGE$
//Unknown:  ERR ERL SPHITT
//Yes:      FUNCNO TCHX TCHY TCHST TCHTIME MAINCNTL MAINCNTH KEYBOARD TIME$ DATE$ 

void Variables::clear_(){
	auto iter = vars.begin();
	while (iter != vars.end()){
		if (sysvars.find(" "+iter->first+" ") == std::string::npos){
			//erase if not sysvar
			iter = vars.erase(iter);
		} else {
			//Can't erase sysvars since some other stuff holds pointers to the internal data
			//(Also, sysvars don't get erased by CLEAR anyway)
			iter++;
		}
	}
}

void Variables::init_sysvars(){
	auto prev = 0;
	
	while (sysvars.find(" ", prev+1) != std::string::npos){
		std::string var = sysvars.substr(prev+1, sysvars.find(" ", prev+1)-prev-1);
		prev = sysvars.find(" ", prev+1);
		
		create_var(var, vars);
	}
	
	//constants
	*std::get<Number*>(get_var_ptr("TRUE")) = 1;
	*std::get<Number*>(get_var_ptr("FALSE")) = 0;
	*std::get<Number*>(get_var_ptr("CANCEL")) = -1;
	*std::get<Number*>(get_var_ptr("VERSION")) = 0x2030; //2.3
	
	//variables
	*std::get<Number*>(get_var_ptr("ICONPMAX")) = 1;
	*std::get<Number*>(get_var_ptr("ICONPAGE")) = 0;
	*std::get<Number*>(get_var_ptr("ICONPUSE")) = 0;
	
	//read-only variables
	*std::get<Number*>(get_var_ptr("MAINCNTL")) = 0;
	*std::get<Number*>(get_var_ptr("MAINCNTH")) = 0;
}

Var Variables::get_sysvar(std::string name){
	std::lock_guard g(sysvar_access);
	return get_var_val(name, {}, vars);
}

/*template<typename T>
void Variables::write_sysvar(std::string name, T val){
	std::lock_guard g(sysvar_access);
	*std::get<T*>(get_var_ptr(name)) = val;
}*/

