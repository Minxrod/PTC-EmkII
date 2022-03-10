#pragma once

#include "Vars.h"
#include <cmath>
#include <random>

namespace ptc {
	
	template <typename T>
	bool is_type(Var v){
		return std::holds_alternative<T>(v);
	}

	void require_same(const std::vector<Var>& vals){
		if (vals.at(0).index() != vals.at(1).index())
			throw std::runtime_error{"mismatched types for binary operator >:("};
	}

	auto get_nums(const std::vector<Var>& vals){
		std::pair<Number, Number> nums {
			std::get<Number>(vals.at(0)), std::get<Number>(vals.at(1))
		};
		return nums;
	}

	Var add(const std::vector<Var>& vals){
		require_same(vals);

		if (is_type<Number>(vals.at(0))){
			auto [n1, n2] = get_nums(vals);;
					
			return Var(n1 + n2);
		} else {
			//assume strings
			auto s1 = std::get<String>(vals.at(0));
			auto s2 = std::get<String>(vals.at(1));
			
			return Var(s1 + s2);	
		}
	}

	Var sub(const std::vector<Var>& vals){
		auto [n1, n2] = get_nums(vals);
			
		return Var(Number{n1 - n2});
	}
	
	Var neg(const Vals& vals){
		return Var(-std::get<Number>(vals.at(0)));
	}
	
	Var mul(const std::vector<Var>& vals){
		if (is_type<Number>(vals.at(0))){
			auto [n1, n2] = get_nums(vals);;
					
			return Var(n1 * n2);
		} else {
			//assume string * num
			auto s = std::get<String>(vals.at(0));
			auto n = std::get<Number>(vals.at(1));
			
			String sf;
			for (int i = 0; i < n; i++)
				sf += s;
			
			return Var(sf);	
		}
	}

	Var div(const std::vector<Var>& vals){
		auto [n1, n2] = get_nums(vals);
			
		return Var(Number{n1 / n2});
	}

	Var mod(const std::vector<Var>& vals){
		auto [n1, n2] = get_nums(vals);
			
		return Var(Number{static_cast<double>(static_cast<int>(n1) % static_cast<int>(n2))});
	}

	Var inv(const std::vector<Var>& vals){		
		return Var(Number{static_cast<double>(!std::get<Number>(vals.at(0)))});
	}

	Var eq(const std::vector<Var>& vals){
		if (is_type<Number>(vals.at(0))){
			auto [n1, n2] = get_nums(vals);
			return Var(Number{static_cast<double>(std::abs(n1 - n2) < (1.0/4096.0))});
		} else {
			auto s1 = std::get<String>(vals.at(0));
			auto s2 = std::get<String>(vals.at(1));
			return Var(Number{static_cast<double>(s1 == s2)});
		}
	}

	Var neq(const std::vector<Var>& vals){
		if (is_type<Number>(vals.at(0))){
			auto [n1, n2] = get_nums(vals);
			return Var(Number{static_cast<double>(std::abs(n1 - n2) >= (1.0/4096.0))});
		} else {
			auto s1 = std::get<String>(vals.at(0));
			auto s2 = std::get<String>(vals.at(1));
			return Var(Number{static_cast<double>(s1 != s2)});
		}
	}

	Var leq(const std::vector<Var>& vals){
		auto [n1, n2] = get_nums(vals);
			
		return Var(Number{static_cast<double>(n1 <= n2)});
	}

	Var geq(const std::vector<Var>& vals){
		auto [n1, n2] = get_nums(vals);
			
		return Var(Number{static_cast<double>(n1 >= n2)});
	}

	Var less(const std::vector<Var>& vals){
		auto [n1, n2] = get_nums(vals);
			
		return Var(Number{static_cast<double>(n1 < n2)});
	}

	Var more(const std::vector<Var>& vals){
		auto [n1, n2] = get_nums(vals);
			
		return Var(Number{static_cast<double>(n1 > n2)});
	}

	Var band(const std::vector<Var>& vals){
		auto [n1, n2] = get_nums(vals);
		
		return Var(Number{static_cast<double>(static_cast<int>(n1) & static_cast<int>(n2))});
	}

	Var bor(const std::vector<Var>& vals){
		auto [n1, n2] = get_nums(vals);
			
		return Var(Number{static_cast<double>(static_cast<int>(n1) | static_cast<int>(n2))});
	}

	Var bnot(const std::vector<Var>& vals){		
		return Var(Number{static_cast<double>(~static_cast<int>(std::get<Number>(vals.at(0))))});
	}

	Var sin(const std::vector<Var>& vals){
		return Var(Number(std::sin(std::get<Number>(vals.at(0)))));
	}

	Var cos(const std::vector<Var>& vals){
		return Var(Number(std::cos(std::get<Number>(vals.at(0)))));
	}

	Var tan(const std::vector<Var>& vals){
		return Var(Number(std::tan(std::get<Number>(vals.at(0)))));
	}

	const double PI = 3.14159265358979;

	Var pi(const std::vector<Var>& vals){
		if (vals.size() > 0)
			throw std::runtime_error{"what why"};	
		return Var(Number(PI)); //todo: determine actual PTC value
	}

	Var abs(const std::vector<Var>& vals){
		return Var(Number(std::abs(std::get<Number>(vals.at(0)))));
	}

	Var asc(const std::vector<Var>& vals){
		return Var(Number((unsigned char)(std::get<String>(vals.at(0))[0])));
	}

	Var atan(const std::vector<Var>& vals){
		if (vals.size() == 1){
			return Var(Number(std::atan(std::get<Number>(vals.at(0)))));
		} else {
			return Var(Number(std::atan(std::get<Number>(vals.at(0))/std::get<Number>(vals.at(1)))));
		}
	}

	Var deg(const std::vector<Var>& vals){
		return Var(Number(std::get<Number>(vals.at(0))/PI*180.0));
	}

	Var rad(const std::vector<Var>& vals){
		return Var(Number(std::get<Number>(vals.at(0))*PI/180.0));
	}
	
	Var rnd(const std::vector<Var>& vals){
		return Var((double)(std::rand() % (int)std::get<Number>(vals.at(0))));
	}
	
	Var len(const Vals& vals){
		return Var((double)std::get<String>(vals.at(0)).size());
	}
	
	Var left(const Vals& vals){
		return Var(std::get<String>(vals.at(0)).substr(0,(int)std::get<Number>(vals.at(1))));
	}

	Var mid(const Vals& vals){
		return Var(std::get<String>(vals.at(0)).substr((int)std::get<Number>(vals.at(1)), (int)std::get<Number>(vals.at(2))));
	}

	Var right(const Vals& vals){
		return Var(std::get<String>(vals.at(0)).substr((int)std::get<Number>(vals.at(1))));
	}
	
	Var val(const Vals& vals){
		return Var((double)std::stoi(std::get<String>(vals.at(0))));
	}
	
	Var chr(const Vals& vals){
		String s = "X";
		s[0] = (char)std::get<Number>(vals.at(0));
		return Var(s);
	}
	
	Var floor(const Vals& vals){
		return Var(std::floor(std::get<Number>(vals.at(0))));
	}
	
	Var sgn(const Vals& vals){
		return Var(std::get<Number>(vals.at(0)) > 0 ? 1.0 : (std::get<Number>(vals.at(0)) < 0 ? -1.0 : 0.0));
	}
	
	Var str(const Vals& vals){
		std::string str = std::to_string(std::get<Number>(vals.at(0)));
		str = str.substr(0, str.find(".")+3);
		while (str.back() == '0'){
			str = str.substr(0, str.size()-1);
		}
		if (str.back() == '.')
			str = str.substr(0, str.size()-1);	
		return Var(str);
	}
	
	Var instr(const Vals& vals){
		std::string str = std::get<String>(vals.at(0));
		std::string substr = std::get<String>(vals.at(1));
		
		auto res = str.find(substr);
		if (res == std::string::npos){
			return Var(-1.0);
		} else {
			return Number(res);
		}
	}
	
	Var subst(const Vals& vals){
		std::string str = std::get<String>(vals.at(0));
		int start = std::get<Number>(vals.at(1));
		int num = std::get<Number>(vals.at(2));
		std::string newstr = std::get<String>(vals.at(3));
		
		str.replace(start, num, newstr);
		return Var(str);
	}
	
}
