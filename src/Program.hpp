#pragma once

#include <vector>
#include <map>
#include <set>
#include <utility>
#include <atomic>

#include "Vars.hpp"
#include "Evaluator.hpp"
#include "FileLoader.hpp"
#include "Resources.hpp"
#include "IPTCObject.hpp"

class Program {
	friend class Debugger; //useful for access to breakpoints, current location, stack, etc.
	
	Evaluator& e;
	
	std::vector<Token> tokens;
	
	/// Maps indexes into tokens to PTC line numbers.
	std::map<int, int> index_to_line{};
	/// Maps PTC line numbers to indexes into tokens.
	std::map<int, int> line_to_index{};
	std::set<int> breakpoints{};
	std::atomic_bool paused{false};
	
	std::vector<Token>::const_iterator current;
	std::vector<Token>::const_iterator data_current;
	
	std::stack<std::vector<Token>::const_iterator> gosub_calls;
	std::vector<std::tuple<Expr, Expr::const_iterator, Expr, Expr>> for_calls;
	
	std::map<Token, cmd_type> commands{};
	
	bool auto_reload = false;
	
	void if_(const Args&);
	void else_(const Args&);
	void for_(const Args&);
	void next_(const Args&);
	void goto_(const Args&);
	void gosub_(const Args&);
	void on_(const Args&);
	void return_(const Args&);
	void end_(const Args&);
	void wait_(const Args&);
	
	void stop_(const Args&);
	void exec_(const Args&);
	
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
	
	Program() = delete;
	
	Program(const Program&) = delete;
	
	Program& operator=(const Program&) = delete;
	
	void set_tokens(const std::vector<Token>&);
	void loader();
	void restart(); //restart current program
	/// Pauses or unpauses program execution on the next instruction.
	/// @param p true=pause, false=unpause
	void pause(bool p) { paused = p; };
	
	void add_cmds(std::map<Token, cmd_type>);
	void call_cmd(Token, const std::vector<std::vector<Token>>&);
	
	std::vector<Token> next_instruction();
	bool at_eof();
	void goto_label(const std::string&);
//	std::vector<std::vector<Token>> split(const std::vector<Token>&);
	
	//start thread
	void run();
	
	void set_breakpoint(int line, bool enable = true);
	
	void _reload(bool state) { auto_reload = state; }
};

std::vector<Token> tokenize(PRG&);
