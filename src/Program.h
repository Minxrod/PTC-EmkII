#pragma once

#include <vector>
#include <map>
#include <set>
#include <utility>

#include "Vars.h"
#include "Evaluator.h"
#include "FileLoader.h"
#include "Resources.h"

class Program{
	Evaluator& e;
	
	const std::vector<Token> tokens;
	std::vector<int> line_starts{};
	std::set<int> breakpoints{};
	
	std::vector<Token>::const_iterator current;
	std::vector<Token>::const_iterator data_current;
	
	std::stack<std::vector<Token>::const_iterator> gosub_calls;
	std::vector<std::tuple<Expr, Expr::const_iterator, Expr, Expr>> for_calls;
	
	std::map<Token, cmd_type> commands{};
		
	void if_(const Args&);
	void for_(const Args&);
	void next_(const Args&);
	void goto_(const Args&);
	void gosub_(const Args&);
	void on_(const Args&);
	void return_(const Args&);
	void end_(const Args&);
	void stop_(const Args&);
	void wait_(const Args&);
	
	void data_(const Args&);
	void read_(const Args&);
	void restore_(const Args&);

	void clear_(const Args&);
	void dim_(const Args&);
	void swap_(const Args&);
	void sort_(const Args&);
	void rsort_(const Args&);
	
	//actual program runner
	void run_();
	
public:
	Program(Evaluator&, const std::vector<Token>&);
	
	void add_cmds(std::map<Token, cmd_type>);
	void call_cmd(Token, const std::vector<std::vector<Token>>&);
	
	std::vector<Token> next_instruction();
	bool at_eof();
	void goto_label(const std::string&);
//	std::vector<std::vector<Token>> split(const std::vector<Token>&);
	
	//start thread
	void run();
	
	void set_breakpoint(int line, bool enable);
};

std::vector<Token> tokenize(PRG&);
