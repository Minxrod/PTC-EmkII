#include "Program.h"

#include <thread>
#include <chrono>

Program::Program(Evaluator& eval, const std::vector<Token>& t) : e{eval}, tokens{t}{
	current = tokens.cbegin();
	
	commands = std::map<Token, cmd_type>{
		cmd_map("FOR"_TC, getfunc<Program>(this, &Program::for_)),
//		cmd_map("IF"_TC, if_),
		cmd_map("NEXT"_TC, getfunc<Program>(this, &Program::next_)),
//		cmd_map("GOTO"_TC, goto_),
//		cmd_map("GOSUB"_TC, gosub_),
//		cmd_map("ON"_TC, on_),
//		cmd_map("RETURN"_TC, return_),
//		cmd_map("STOP"_TC, &Program::stop_),
//		cmd_map("END"_TC, &Program::end_),
		cmd_map("WAIT"_TC, getfunc<Program>(this, &Program::wait_)),
//		cmd_map("CLS"_TC, getfunc<Program>(this, &Program::debugprint)),
//		cmd_map("PRINT"_TC, getfunc<Program>(this, &Program::debugprint)),
	};
}

void Program::add_cmds(std::map<Token, cmd_type> other){
	commands.merge(other);
}

std::vector<Token> Program::next_instruction(){
	Expr::const_iterator newline;
	if (*current == "IF"_TC){ //IF should read entire line
		newline = std::find(current, tokens.end(), Token{"\r", Type::Newl});
	} else {
		newline = std::min(std::find(current, tokens.end(), Token{"\r", Type::Newl}),
							std::find(current, tokens.end(), Token{":", Type::Newl}));
	}
	std::vector<Token> instr{current, newline};
	current = newline+1;
	return instr;
}

void Program::goto_label(const std::string& label){
	current = std::find(tokens.begin(), tokens.end(), Token{label, Type::Label});
}

bool Program::at_eof(){
	return current == tokens.cend();
}

std::vector<std::vector<Token>> Program::split(const std::vector<Token>& expression){
	std::vector<std::vector<Token>> subexp{};
	
	std::vector<PrioToken> all = conv_tokens(expression);
	
	auto i = 0;
	auto start = expression.begin();
	auto old = i;
	while (i < (int)all.size()){
		if (all.at(i).type == Type::Cmd){
			if (old != i) //ON ... GOTO IF ... THEN 
				subexp.push_back(std::vector<Token>(start+old, start+i));
			subexp.push_back(std::vector<Token>{all.at(i)}); //some command
			old=i+1;
		}
		if (all.at(i).prio == 1){
			//found low-prio comma
			subexp.push_back(std::vector<Token>(start+old, start+i));
			old=i+1;
		}
		i++;
	}
	//this works since there should always be at least one subexp.
	if (i != old)
		subexp.push_back(std::vector<Token>(start+old,start+i));
		
	return subexp;
}

void Program::call_cmd(Token instr, const Args& chunks){
	commands.at(instr)(chunks);
}

void Program::run(){
	current = tokens.cbegin();
	std::cout << "\nbegin run\n" << std::endl;
	
	while (current != tokens.cend()){
		auto instr = next_instruction();
		if (instr.empty())
			continue; //it's an empty line, don't try to run it
		
		auto chunks = split(instr);
		auto instr_form = chunks[0][0]; //if chunks[0] is empty, we have other problems
		
		for (auto chunk : chunks)
			print("Instr:", chunk);
		
/*		if (instr_form.type == Type::Rem){ //ignore it, this is the entire line
		} else if (instr_form.type == Type::Label){ //ignore
		}else*/if (instr_form.type == Type::Num){ //error
		} else if (instr_form.type == Type::Str){ //error
		} else if (instr_form.type == Type::Arr){ //check for valid assignment
			if (chunks.size() == 1){
				//probably valid
				e.evaluate(chunks[0]);
			}
		} else if (instr_form.type == Type::Var){ //check for assignment
			if (chunks.size() == 1){
				//probably valid
				e.evaluate(chunks[0]);
			}
		} else if (instr_form.type == Type::Op){ //error
		} else if (instr_form.type == Type::Func){ //error
		} else if (instr_form.type == Type::Cmd){ //run cmd
			if (instr_form.text == "IF"){
				call_cmd(instr_form, std::vector<Expr>{instr});
			} else {
				call_cmd(instr_form, chunks);
			}
		//} else if (instr_form.type == Type::Newl){ //ignore
		} else { //something has gone horrifically wrong
		}
	}
	
	std::cout << "Program.run() end" << std::endl;
}

void Program::debugprint(const Args& a){
	for (auto exp : a)
		print("dubug:"+exp[0].text, exp);
}

void Program::for_(const Args& a){
	//FOR <initial> TO <value> [STEP <value>]
	auto& init = a[1]; //initial
	auto eq = std::find(init.begin(), init.end(), "="_TO);
	auto var = std::vector<Token>{init.begin(), eq}; //read until = to get var expression
	
	auto& end = a[3]; //end value
	auto cond = std::vector<Token>{var};
	cond.push_back("<="_TO);
	cond.insert(cond.end(), end.begin(), end.end());
	
	//buiild <var>=<var>+<step> expression
	auto step = std::vector<Token>{var};
	step.push_back("="_TO);
	step.insert(step.end(), var.begin(), var.end());
	step.push_back("+"_TO);
	if (a.size() == 6){
		step.insert(step.end(), a[5].begin(), a[5].end());
	} else {
		step.push_back(Token{"1", Type::Num}); //default step is 1
	}
	
	print("init:", init);
	print("cond:", cond);
	print("step:", step);
	
	e.evaluate(init); //run init condition
	for_calls.push_back(std::make_tuple(var, current, cond, step)); //needed for looping
}

void Program::next_(const Args& a){
	std::vector<std::tuple<Expr, Expr::const_iterator, Expr, Expr>>::iterator itr;
	if (a.size() > 1){
		//a contains a variable name, referring to a specifc for loop
		//search for specific vector of var name
		itr = std::find_if(for_calls.begin(), for_calls.end(), [&a](auto& x){ return std::get<0>(x) == a[1];});
	} else {
		itr = for_calls.end()-1;
	}
	
	e.evaluate(std::get<3>(*itr)); //run step
	auto res = e.evaluate(std::get<2>(*itr)); //check condition
	if (std::abs(std::get<Number>(res)) < 0.000244140625){
		//false: loop ends
		for_calls.erase(itr); //remove from ""stack"" when loop ends
	} else {
		//true: loop continues
		current = std::get<1>(*itr);
	}
}

void Program::wait_(const Args& a){
	auto frames = 1000.0 / 60.0 * std::get<Number>(e.evaluate(a[1]));
	
	std::this_thread::sleep_for(std::chrono::milliseconds((int)frames));
}

void Program::if_(const Args& a){
	//IF <cond> THEN CMD A,B:CMD C,D ELSE CMD E\r
	//IF, <cond>, THEN, CMD, A, B, : CMD, C, D, ELSE, CMD, E, \r
	//<nextline>
	//correct code will look like this (can be GOTO or THEN!)
	auto& line = a[0]; //if is special case, all data in one chunk
	
	auto pass = std::find_if(line.begin(), line.end(), [](auto& x){return x=="THEN"_TC || x=="GOTO"_TC;});
	//from IF <COND> THEN/GOTO
	Expr cond = Expr(line.begin()+1, pass);
	
	int nest = 0;
	//needed to handle stuff like IF A THEN IF B THEN ?"B" ELSE ?"A" ELSE ?"0"
	auto if_else = [&nest](auto& i){
		if (i == "IF"_TC)
			nest++;
		if (i == "ELSE"_TC){
			if (nest > 0){
				nest--;
			} else {
				return true;
			}
		}
		return false;
	};
	auto fail = std::find_if(pass+1, line.end(), if_else); //else (might not exist)
	
	auto res = e.evaluate(cond); //condition
	if (std::abs(std::get<Number>(res)) > 0){
		current = current - std::distance(pass, line.end());
	} else {
		if (fail != line.end()){ //else was found
			current = current - std::distance(fail, line.end());
		} //else not found, current is already past instruction (no change needed)
	}

}

const std::regex string{ R"("[^]*("|\r))" };
const std::regex number{ R"(([0-9]*\.)?[0-9]+)" };
const std::regex separator{ R"([:\r])" };
const std::regex variable{ R"([A-Z_]+\$?)" };
const std::regex label{ R"(\@[A-Z0-9_]+)" };
const std::regex comment{ R"('.*\r)" };
const std::string first_char_ops = ",;[]()+-*/%!<>="; //single character operations or leading characters
const std::string second_char_ops = "<=>"; //second character of operations

const std::string commands{" ACLS APPEND BEEP BGCLIP BGCLR BGCOPY BGFILL BGMCLEAR BGMPLAY BGMPRG BGMSET BGMSETD BGMSETV BGMSTOP BGMVOL BGOFS BGPAGE BGPUT BGREAD BREPEAT CHRINIT CHRREAD CHRSET CLEAR CLS COLINIT COLOR COLREAD COLSET CONT DATA DELETE DIM DTREAD ELSE END EXEC FOR GBOX GCIRCLE GCLS GCOLOR GCOPY GDRAWMD GFILL GLINE GOSUB GOTO GPAGE GPAINT GPSET GPRIO GPUTCHR ICONCLR ICONSET IF INPUT KEY LINPUT LIST LOAD LOCATE NEW NEXT NOT ON PNLSTR PNLTYPE PRINT READ REBOOT RECVFILE REM RENAME RESTORE RETURN RSORT RUN SAVE SENDFILE SORT SPANGLE SPANIM SPCHR SPCLR SPCOL SPCOLVEC SPHOME SPOFS SPPAGE SPREAD SPSCALE SPSET SPSETV STEP STOP SWAP THEN TMREAD TO VISIBLE VSYNC WAIT "};

const std::string functions{" ABS ASC ATAN BGCHK BGMCHK BGMGETV BTRIG BUTTON CHKCHR CHR$ COS DEG EXP FLOOR GSPOIT HEX$ ICONCHK INKEY$ INSTR LEFT$ LEN LOG MID$ PI POW RAD RIGHT$ RND SGN SIN SPCHK SPGETV SPHIT SPHITRC SPHITSP SQR STR$ SUBST$ TAN VAL "};

const std::string operations{" AND NOT OR ! - + - * / = == => =< < > != % ( ) [ ] , ; "};

std::vector<Token> tokenize(PRG& prg){
	size_t char_pos = 0;

	std::string cur{};
	
	std::vector<Token> tokens{};
	std::vector<unsigned char>& data = prg.data;

	auto loop_until_regex = [&](std::regex re, Type tt) 
	{
		do
		{
			cur += prg.data[char_pos];
			char_pos++;
		} while (!std::regex_match(cur, re));
		//add chars until finding a matching string.
		//char_pos is left at the start of the next token.
		tokens.push_back(Token{ std::string(cur), tt });
	};

	auto loop_while_regex = [&](std::regex re, Type tt)
	{
		do
		{
			cur += prg.data[char_pos];
			char_pos++;
		} while (std::regex_match(cur, re));
		//add chars until regex no longer matches
		//when pattern fails, remove failing character
		cur = cur.substr(0, cur.size() - 1);
		char_pos--;

		tokens.push_back(Token{ std::string(cur), tt });
	};

	while (char_pos < prg.data.size())
	{
		cur.clear();
		char c = data[char_pos];

		if (c == '"') //strings
		{
			do
			{
				cur += data[char_pos];
				char_pos++;
			} while (!std::regex_match(cur, string));
			//add to list with no quotes
			tokens.push_back(Token{ cur.substr(1, cur.size() - 2), Type::Str });

			if (data[char_pos-1] == '\r')
				--char_pos; //needs \r to mark end of instruction line

		}
		else if (c == '\'') //comments
		{
			loop_until_regex(comment, Type::Rem);
			char_pos--; //don't include newline >_>
		}
		else if (c == ':' || c == '\r') //separators
		{
			loop_until_regex(separator, Type::Newl);
		}
		else if (c == '@')
		{
			cur += c;
			char_pos++; //to match regex, needs @ and at least one character after
			loop_while_regex(label, Type::Label);
		}
		else if ((c >= '0' && c <= '9') || c == '.') //numbers
		{
			loop_while_regex(number, Type::Num);
		}
		else if ((c <= 'Z' && c >= 'A') || (c <= 'z' && c >= 'a'))
		{
			do
			{
				cur += data[char_pos] <= 'z' && data[char_pos] >= 'a' ?
					data[char_pos] - 'a' + 'A' : //make all capitals
					data[char_pos]; //don't change numbers, symbols, etc.

				char_pos++;

			} while (std::regex_match(cur, variable));
			cur = cur.substr(0, cur.size() - 1);
			--char_pos;
			//assume variable type if not matching any commands, etc.
			Type tt = Type::Var;
			if (commands.find(" "+cur+" ") != std::string::npos)
				tt = Type::Cmd;
			else if (functions.find(" "+cur+" ") != std::string::npos)
				tt = Type::Func;
			else if (operations.find(" "+cur+" ") != std::string::npos)
				tt = Type::Op;
			
			tokens.push_back(Token{ std::string(cur), tt });
		}
		else if (first_char_ops.find(c) != std::string::npos)
		{
			cur += data[char_pos];
			++char_pos;

			//check for double-symbol tokens (compare ops)
			if ((c == '=' || c == '!') && second_char_ops.find(data[char_pos]) != std::string::npos)
			{
				cur += data[char_pos];
				++char_pos;
			}
			if (c == '(' || c == '['){
				//if finding parens directly after a variable, must be an array.
				if (tokens.back().type == Type::Var)
					tokens.back().type = Type::Arr;
			}
			
			tokens.push_back(Token{ std::string(cur), Type::Op });
		}
		else if (c == '?')
		{
			++char_pos;
			tokens.push_back(Token{ std::string("PRINT"), Type::Cmd });
		}
		else
		{
			++char_pos;
		}
	}
	
	return tokens;
}
