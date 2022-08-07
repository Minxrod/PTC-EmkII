#include "Tokens.hpp"
#include "Vars.hpp"

Expr tokenize_str(std::string);

template <typename Out>
auto wrap_ptc_func(Var(* f)(const Vals&)){
	return [f](const Vals& v) -> Out{
		return std::get<Out>(f(v));
	};
}
