#pragma once

#include <vector>
#include <map>
#include <utility>

#include "Vars.h"
#include "Evaluator.h"
#include "FileLoader.h"
#include "Resources.h"

class Program{
	Evaluator& e;
	
	const std::vector<Token> tokens;
	std::vector<Token>::const_iterator current;
	
	std::stack<std::vector<Token>::const_iterator> gosub_calls;
	std::vector<std::tuple<Expr, Expr::const_iterator, Expr, Expr>> for_calls;

	std::map<Token, cmd_type> commands;
	
	void debugprint(const Args&);
	
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

	//actual program runner
	void run_();
	
public:
	Program(Evaluator&, const std::vector<Token>&);
	
	void add_cmds(std::map<Token, cmd_type>);
	void call_cmd(Token, const std::vector<std::vector<Token>>&);
	
	std::vector<Token> next_instruction();
	bool at_eof();
	void goto_label(const std::string&);
	std::vector<std::vector<Token>> split(const std::vector<Token>&);
	
	//start thread
	void run();
};

std::vector<Token> tokenize(PRG&);
