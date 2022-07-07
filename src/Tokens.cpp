#include "Tokens.hpp"

#include <regex>

bool operator<(const Token& a, const Token& b){
	return a.text < b.text ? true : b.text < a.text ? false : a.type < b.type;
}

bool operator<(const PrioToken& a, const PrioToken& b){
	return a.prio < b.prio;
}

bool operator==(const Token& a, const Token& b){
	return a.type == b.type && a.text == b.text;
}

Token operator""_TO(const char* x, std::size_t y){
	return Token{std::string{x, y}, Type::Op};
}

Token operator""_TF(const char* x, std::size_t y){
	return Token{std::string{x, y}, Type::Func};
}

Token operator""_TC(const char* x, std::size_t y){
	return Token{std::string{x, y}, Type::Cmd};
}

const std::regex string{ R"("[^]*("|\r))" };
const std::regex number{ R"([0-9]*\.?[0-9]*)" };
const std::regex hex{ R"(\&H[0-9A-Fa-f]*)" }; //does not allow decimal points
const std::regex binary{ R"(\&B[01]*)" }; //does not allow decimal points
const std::regex separator{ R"([:\r])" };
const std::regex variable{ R"([A-Z_][A-Z0-9_]*\$?)" };
const std::regex label{ R"(\@[A-Z0-9_]+)" };
const std::regex comment{ R"(('|REM).*\r)" };
const std::string first_char_ops = ",;[]()+-*/%!<>="; //single character operations or leading characters
const std::string second_char_ops = "="; //second character of operations

const std::string commands{" ACLS APPEND BEEP BGCLIP BGCLR BGCOPY BGFILL BGMCLEAR BGMPLAY BGMPRG BGMSET BGMSETD BGMSETV BGMSTOP BGMVOL BGOFS BGPAGE BGPUT BGREAD BREPEAT CHRINIT CHRREAD CHRSET CLEAR CLS COLINIT COLOR COLREAD COLSET CONT DATA DELETE DIM DTREAD ELSE END EXEC FOR GBOX GCIRCLE GCLS GCOLOR GCOPY GDRAWMD GFILL GLINE GOSUB GOTO GPAGE GPAINT GPSET GPRIO GPUTCHR ICONCLR ICONSET IF INPUT KEY LINPUT LIST LOAD LOCATE NEW NEXT NOT ON PNLSTR PNLTYPE PRINT READ REBOOT RECVFILE REM RENAME RESTORE RETURN RSORT RUN SAVE SENDFILE SORT SPANGLE SPANIM SPCHR SPCLR SPCOL SPCOLVEC SPHOME SPOFS SPPAGE SPREAD SPSCALE SPSET SPSETV STEP STOP SWAP THEN TMREAD TO VISIBLE VSYNC WAIT "};

const std::string functions{" ABS ASC ATAN BGCHK BGMCHK BGMGETV BTRIG BUTTON CHKCHR CHR$ COS DEG EXP FLOOR GSPOIT HEX$ ICONCHK INKEY$ INSTR LEFT$ LEN LOG MID$ PI POW RAD RIGHT$ RND SGN SIN SPCHK SPGETV SPHIT SPHITRC SPHITSP SQR STR$ SUBST$ TAN VAL "};

const std::string operations{" XOR AND NOT OR ! - + - * / = == >= <= < > != % ( ) [ ] , ; "};

std::vector<Token> tokenize(unsigned char* data, std::size_t size){
	std::size_t char_pos = 0;

	std::string cur{};
	
	std::vector<Token> tokens{};

	auto loop_until_regex = [&](std::regex re, Type tt) 
	{
		do
		{
			cur += data[char_pos] <= 'z' && data[char_pos] >= 'a' ?
				data[char_pos] - 'a' + 'A' : //make all capitals
				data[char_pos]; //don't change numbers, symbols, etc.

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
			cur += data[char_pos] <= 'z' && data[char_pos] >= 'a' ?
				data[char_pos] - 'a' + 'A' : //make all capitals
				data[char_pos]; //don't change numbers, symbols, etc.

			char_pos++;
		} while (std::regex_match(cur, re));
		//add chars until regex no longer matches
		//when pattern fails, remove failing character
		cur = cur.substr(0, cur.size() - 1);
		char_pos--;

		tokens.push_back(Token{ std::string(cur), tt });
	};

	while (char_pos < size)
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
		else if (c == '&'){
			cur += c;
			char_pos++;
			c = data[char_pos] <= 'z' && data[char_pos] >= 'a' ?
				data[char_pos] - 'a' + 'A' : //make all capitals
				data[char_pos];
			cur += c;
			char_pos++;
			if (c == 'H' || c == 'B'){
				loop_while_regex(c == 'H' ? hex : binary, Type::Num);
			}
		}
		else if ((c <= 'Z' && c >= 'A') || (c <= 'z' && c >= 'a') || c == '_')
		{
			do
			{
				cur += data[char_pos] <= 'z' && data[char_pos] >= 'a' ?
					data[char_pos] - 'a' + 'A' : //make all capitals
					data[char_pos]; //don't change numbers, symbols, etc.

				char_pos++;

			} while (char_pos < size && std::regex_match(cur, variable));
			if (!std::regex_match(cur, variable)){ //only trim not-matching character, don't trim last valid character
				cur = cur.substr(0, cur.size() - 1);
				--char_pos;
			}
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
			if ((c == '<' || c == '>' || c == '=' || c == '!') && data[char_pos] == '=')
			{
				cur += data[char_pos];
				++char_pos;
			}
			if (c == '(' || c == '['){
				//if finding parens directly after a variable, must be an array.
				if (tokens.size() && tokens.back().type == Type::Var)
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


std::ostream& print(std::string name, const std::vector<Token>& items){
	std::cout << name << ":" << std::endl;
	for (auto i : items){
		if (i.text != "\r")
			std::cout << i.text << " ";
	}
	return std::cout << std::endl;
}

std::ostream& print(std::string name, const std::vector<PrioToken>& items){
	std::cout << name << ":" << std::endl;
	for (auto i : items){
		std::cout << i.text << "[" << i.prio << "] ";
	}
	return std::cout << std::endl;
}


