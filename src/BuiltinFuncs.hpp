#pragma once

#include "Vars.hpp"

namespace ptc {
	Var add(const std::vector<Var>& vals);
	
	Var sub(const std::vector<Var>& vals);
	
	Var neg(const Vals& vals);
	
	Var mul(const std::vector<Var>& vals);
	
	Var div(const std::vector<Var>& vals);
	
	Var mod(const std::vector<Var>& vals);
	
	Var inv(const std::vector<Var>& vals);
	
	Var eq(const std::vector<Var>& vals);
	
	Var neq(const std::vector<Var>& vals);
	
	Var leq(const std::vector<Var>& vals);
	
	Var geq(const std::vector<Var>& vals);
	
	Var less(const std::vector<Var>& vals);
	
	Var more(const std::vector<Var>& vals);
	
	Var band(const std::vector<Var>& vals);
	
	Var bor(const std::vector<Var>& vals);
	
	Var bnot(const std::vector<Var>& vals);
	
	Var bxor(const std::vector<Var>& vals);
	
	Var sin(const std::vector<Var>& vals);
	
	Var cos(const std::vector<Var>& vals);
	
	Var tan(const std::vector<Var>& vals);
	
	Var pi(const std::vector<Var>& vals);
	
	Var abs(const std::vector<Var>& vals);
	
	Var asc(const std::vector<Var>& vals);
	
	Var atan(const std::vector<Var>& vals);
	
	Var deg(const std::vector<Var>& vals);
	
	Var rad(const std::vector<Var>& vals);
	
	Var rnd(const std::vector<Var>& vals);
	
	Var len(const Vals& vals);
	
	Var left(const Vals& vals);
	
	Var mid(const Vals& vals);
	
	Var right(const Vals& vals);
	
	Var val(const Vals& vals);
	
	Var chr(const Vals& vals);
	
	Var floor(const Vals& vals);
	
	Var sgn(const Vals& vals);
	
	Var str(const Vals& vals);
	
	Var instr(const Vals& vals);
	
	Var subst(const Vals& vals);
	
	Var hex(const Vals& vals);
	
	Var pow(const Vals& vals);
}
