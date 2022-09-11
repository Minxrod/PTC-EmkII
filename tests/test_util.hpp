#pragma once

#include "Tokens.hpp"
#include "Vars.hpp"

#include "PTC2Console.hpp"

Expr tokenize_str(std::string);

template <typename Out>
auto wrap_ptc_func(Var(* f)(const Vals&)){
	return [f](const Vals& v) -> Out{
		return std::get<Out>(f(v));
	};
}

void debug_print(const PTC2Console& con);

Number debug_eval(PTCSystem& system, const std::string expr);
