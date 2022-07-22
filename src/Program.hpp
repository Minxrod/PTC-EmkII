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

class PTCSystem;

/// Program execution and management class.
/// 
/// Manages the execution of a program, flow control, and `DATA` information.
/// Additionally, handles partss of some extra related functions such as program reloading and breakpoints.
class Program {
	friend class Debugger; //useful for access to breakpoints, current location, stack, etc.
	
	/// Containing system
	PTCSystem* system;
	/// Evaluator object
	Evaluator& e;
	
	/// The tokens of the current program
	std::vector<Token> tokens;
	
	/// Maps indexes into tokens to PTC line numbers.
	std::map<int, int> index_to_line{};
	/// Maps PTC line numbers to indexes into tokens.
	std::map<int, int> line_to_index{};
	/// Breakpoint line numbers.
	std::set<int> breakpoints{};
	/// Is the program currently executing?
	std::atomic_bool paused{false};
	
	/// Current instruction iterator into tokens
	std::vector<Token>::const_iterator current;
	/// Iterator pointing to next `DATA` item to read
	std::vector<Token>::const_iterator data_current;
	
	/// Stack of GOSUB calls.
	std::stack<std::vector<Token>::const_iterator> gosub_calls;
	/// Stack of FOR locations.
	/// Each entry is a tuple of form (variable expression, instruction iterator, end expression, step expression)
	std::vector<std::tuple<Expr, Expr::const_iterator, Expr, Expr>> for_calls;
	
	/// Map of tokens to instructions.
	std::map<Token, cmd_type> commands{};
	
	/// Should the program automatically restart after ending?
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
	
public:
	/// Constructor
	/// 
	/// @param s Containing system
	Program(PTCSystem* s);
	
	/// Default constructor (deleted)
	Program() = delete;
	
	/// Copy constructor (deleted)
	Program(const Program&) = delete;
	
	/// Copy assignment (deleted)
	Program& operator=(const Program&) = delete;
	
	/// Sets the `DATA` pointer to the first `DATA` item after label.
	/// 
	/// @param label Label to search for
	void data_seek(std::string label);
	/// Read one `DATA` item.
	/// 
	/// @return `DATA` item
	Token read_expr();
	
	/// Sets the program's tokens.
	/// 
	/// @param t Tokens
	void set_tokens(const std::vector<Token>& t);
	
	/// Loads the default program loader program.
	void loader();
	/// Reloads the current program.
	void restart();
	
	/// Pauses or unpauses program execution on the next instruction.
	/// @param p true=pause, false=unpause
	void pause(bool p) { paused = p; };
	
	/// Adds commands to the command map.
	/// @todo Replace argument with IPTCObject
	/// 
	/// @param commands Commands to add
	void add_cmds(std::map<Token, cmd_type> commands);
	
	/// Executes an instruction.
	/// 
	/// @param instr Instruction
	/// @param args List of argument expressions
	void call_cmd(Token instr, const std::vector<std::vector<Token>>& args);
	
	/// Read the next instruction (starting from current)
	/// 
	/// @return Instruction expression
	std::vector<Token> next_instruction();
	
	/// Check if program execution has reached the end.
	/// 
	/// @return current is at tokens.end()
	bool at_eof();
	/// Jumps program execution to a label
	/// 
	/// @param label Label to jump to
	void goto_label(const std::string& label);
//	std::vector<std::vector<Token>> split(const std::vector<Token>&);
	
	/// Starts the program in a new thread.
	void run();
	/// Runs the program in the current thread.
	void run_();
	
	/// Sets or removes a breakpoint.
	/// 
	/// @param line Line to break on
	/// @param enable true=Set breakpoint, false=Clear breakpoint
	void set_breakpoint(int line, bool enable = true);
	
	/// Sets the state of program auto-reload.
	/// 
	/// @param state true=Enable auto-reload
	void _reload(bool state) { auto_reload = state; }
};

/// Tokenizes from a PRG object.
/// 
/// @param p PRG object
/// @return Tokenized list
std::vector<Token> tokenize(PRG& p);
