#include <string>
#include <sstream>
#include <vector>
#include <regex>

#include "Resources.h"
#include "Vars.h"

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

const std::string operations{" AND NOT OR ! - + - * / == => =< < > != % ( ) [ ] , ; "};

const std::string sysvars{" CSRX CSRY FREEMEM VARSION ERR ERL RESULT TCHX TCHY TCHST TCHTIME MAINCNTL MAINCNTH TABSTEP TRUE FALSE CANCEL ICONPUSE ICONPAGE ICONPMAX FUNCNO FREEVAR SYSBEEP KEYBOARD SPHITNO SPHITX SPHITY SPHITT TIME$ DATE$ MEM$ PRGNAME$ PACKAGE$ "};

Program::Program(const std::vector<Token>& t) : tokens{t}{
	current = tokens.cbegin();
}

std::vector<Token> Program::next_instruction(){
	auto newline = std::min(std::find(current, tokens.end(), Token{"\r", Type::Newl}),
							std::find(current, tokens.end(), Token{":", Type::Newl}));
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
