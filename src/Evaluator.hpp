#pragma once

#include "Vars.hpp"
#include "Variables.hpp"

#include <stack>
#include <string>
#include <vector>
#include <map>

//values don't really matter, just need to be negative and unique
const int INTERNAL_ENDLIST = -999;
const int INTERNAL_SUBEXP = -888;
const int COMMENT = -77;
const int INTERNAL_PAREN = -24;

/// Converts a list of Tokens into a list of PrioTokens.
/// 
/// This function iterates over the list of tokens and assigns a priority to each,
/// based on the contents of the token. Operator priority is as follows, from lowest to highest:
/// * ,
/// * AND OR XOR
/// * == != < > <= >=
/// * + -(subtraction)
/// * * / %
/// * NOT ! -(negation, represented as 0-)
/// * Functions
/// * ( ) [ ]
/// 
/// Other tokens such as variable names will be assigned a priority based on how nested they are within parenthesis.
/// 
/// The special Token "."_TO will be assigned a special priority, as it is used internally by some other functions.
/// 
/// @param t Vector of Tokens
/// @return Vector of PrioTokens, with priority based on t
std::vector<PrioToken> conv_tokens(const std::vector<Token>& t);

/// Converts the given token into a PTC type value.
/// 
/// String and label tokens get converted to Strings; numbers become Numbers.
/// Anything else is considered invalid input and should never happen.
/// 
/// @param t Token to convert
/// @return Var containing either a Number or a String
Var convert_to_value(const Token& t);

/// Tokenizes the given data as a PTC2 program.
/// 
/// @note This was some of the first code created for this iteration of the project, 
/// and could probably be more efficient.
/// 
/// @param data Pointer to PRG data to tokenize
/// @param size Length of data pointed to
/// @return Vector of Tokens
std::vector<Token> tokenize(unsigned char* data, std::size_t size);

/// Splits a list of Tokens on every un-nested comma and Cmd Token.
/// 
/// This function splits a list of Tokens into a list of lists of Tokens,
/// and removes the commas it splits at. Commas within parenthesis are ignored.
/// 
/// This function is used for command/function argument parsing.
/// Example: ["LOCATE","6",",","3","+","2"] -> [["LOCATE"],["6"],["3","+","2"]]
/// 
/// @param t List of Tokens to split
/// @return List of list of Tokens (no un-nested commas)
std::vector<std::vector<Token>> split(const std::vector<Token>& t);

/// The big calculator class that handles most of the actual work.
/// 
/// Evaluator is the struct responsible for taking in a tokenized PTC expression
/// and calculating a result from that. This requires access to variables, and
/// several functions that are defined by other classes.
/// 
/// Every class that implements a PTC command or function interface depends
/// on this class, because any command or function can have expressions as 
/// arguments that must be evaluated.
///
/// @note This class could use some cleanup.
struct Evaluator {
	/// Manages all PTC variables, including system variables.
	Variables vars;
	/// Function objects corresponding to operators, such as +, -, or !.
	std::map<Token, op_func> operators;
	/// Function objects corresponding to PTC functions.
	std::map<Token, op_func> functions;
	/// Map of unprocessed expressions to processed expressions.
	/// This saves time, since process() is not very efficient.
	std::map<std::vector<Token>, std::vector<Token>> processed; 
	
	/// Adds the functions from the provided map to the Evaluator functions map.
	/// @param other Map to be merged with functions
	void add_funcs(std::map<Token, op_func> other);
	
	/// Assigns the value of the Token to the given variable expression.
	/// 
	/// Essentially, this performs exp = t.
	/// 
	/// @param exp Variable name expression
	/// @param t Token representing value to assign
	void assign(const Expr& exp, Token t);
	
	/// Calls an operator with a stack used to determine arguments.
	/// 
	/// @param op Operator to use
	/// @param values Stack containing values for calculation
	/// @return Result of calculation
	Var call_op(const Token& op, std::stack<Var>& values);
	
	/// Calls a function with the given arguments.
	/// 
	/// @param f Function to call
	/// @param args Arguments to use for calculation
	/// @return Result of calculation
	Var call_func(const Token& f, std::vector<Var>& args);
	
	/// Takes an expression and calculates the result.
	/// 
	/// If this expression is new, the processed form is added to processed
	/// to make future calculations faster.
	/// 
	/// @param expression Expression to evaluate
	/// @return Result of calculation
	Var evaluate(const std::vector<Token>& expression);
	
	/// Takes an expression and calculates the result.
	/// 
	/// This functions does NOT check or modify processed.
	/// As a result, this function will be much slower.
	/// 
	/// @param expression Expression to evaluate
	/// @return Result of calculation
	Var eval_no_save(const std::vector<Token>& expression);
	
	/// Takes an expression and calculates the result, including extra results
	/// that would otherwise be discarded.
	/// 
	/// This function affects processed as evaluate() does.
	/// 
	/// This function is used for strange cases like PRINT, where
	/// `PRINT "HI"82934"HELLO"` is completely valid, but evaluate() would
	/// return only "HI".
	/// 
	/// @note The order of results may be reversed from what is expected.
	/// This is due to a stack->vector conversion.
	/// 
	/// @param expression Expression to evaluate
	/// @return Results of calculation, as a list.
	std::vector<Var> eval_all_results(const std::vector<Token>& expression);
	
	/// Takes an expression and converts it into an RPN-approximate form.
	/// 
	/// This functions takes a tokenized expression and converts it to a
	/// form resembling reverse polish notation. Note that this conversion is
	/// likely not optimal, as this was created without much reference.
	/// 
	/// Example: process(["5","+","3"]) -> ["5","3","+"]
	/// @note See https://en.wikipedia.org/wiki/Reverse_Polish_notation
	/// @note Also see https://en.wikipedia.org/wiki/Shunting-yard_algorithm,
	/// which this solution is based on.
	/// 
	/// @param expression Expression to convert to RPN
	/// @return RPN form of expression.
	std::vector<Token> process(const std::vector<Token>&);
	
	/// Given an RPN expression, calculates the result. 
	/// 
	/// This function also has the extra purpose of handling variable 
	/// assignment and array declarations (if do_array_init==true).
	/// 
	/// @param rpn_expression Expression to evaluate
	/// @param do_array_init If true, allows creation of arrays.
	std::vector<Var> calculate(const std::vector<Token>& rpn_expression, bool do_array_init = false);
	
	// PTC commands.
	void dtread_(const Args&);
	void tmread_(const Args&);
	
	/// Constructor.
	Evaluator();
	
	/// Copy constructor (deleted)
	Evaluator(const Evaluator&) = delete;
	
	/// Copy assignment (deleted)
	Evaluator& operator=(const Evaluator&) = delete;
};
