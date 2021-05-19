#include "Program.h"

#include <thread>
#include <chrono>

Program::Program(Evaluator& eval, const std::vector<Token>& t) : e{eval}, tokens{t}{
	current = tokens.cbegin();
	data_current = std::find(tokens.begin(), tokens.end(), "DATA"_TC);
	if (data_current != tokens.end())
		data_current++; //first piece of data will be directly after DATA statement
	
	commands = std::map<Token, cmd_type>{
		cmd_map("FOR"_TC, getfunc(this, &Program::for_)),
		cmd_map("IF"_TC, getfunc(this, &Program::if_)),
		cmd_map("NEXT"_TC, getfunc(this, &Program::next_)),
		cmd_map("GOTO"_TC, getfunc(this, &Program::goto_)),
		cmd_map("GOSUB"_TC, getfunc(this, &Program::gosub_)),
		cmd_map("ON"_TC, getfunc(this, &Program::on_)),
		cmd_map("RETURN"_TC, getfunc(this, &Program::return_)),
//		cmd_map("STOP"_TC, &Program::stop_),
		cmd_map("END"_TC, getfunc(this, &Program::end_)),
		cmd_map("WAIT"_TC, getfunc(this, &Program::wait_)),
		cmd_map("DATA"_TC, getfunc(this, &Program::data_)),
		cmd_map("CLEAR"_TC, getfunc(this, &Program::clear_)),
		cmd_map("DIM"_TC, getfunc(this, &Program::dim_)),
		cmd_map("READ"_TC, getfunc(this, &Program::read_)),
		cmd_map("DTREAD"_TC, getfunc(&eval, &Evaluator::dtread_)),
		cmd_map("TMREAD"_TC, getfunc(&eval, &Evaluator::tmread_))
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

bool Program::at_eof(){
	return current == tokens.cend();
}

void Program::call_cmd(Token instr, const Args& chunks){
	commands.at(instr)(chunks);
}

void Program::run_(){
	current = tokens.cbegin();
	std::cout << "\nbegin run\n" << std::endl;
	
	while (current != tokens.cend()){
		auto instr = next_instruction();
		if (instr.empty())
			continue; //it's an empty line, don't try to run it
		
		auto chunks = split(instr);
		auto instr_form = chunks[0][0]; //if chunks[0] is empty, we have other problems
		
		//for (auto& chunk : chunks)
		//	print("Instr:", chunk);
		
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
	
	commands.at("OK"_TC)({});
	std::cout << "Program end" << std::endl;
}

void Program::run(){
	std::thread t{&Program::run_, this};
	t.detach();
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
	
//	print("init:", init);
//	print("cond:", cond);
//	print("step:", step);
	
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
		if (current->type == Type::Label){
			goto_label(current->text);
		}
	} else {
		if (fail != line.end()){ //else was found
			current = current - std::distance(fail, line.end());
			if (current->type == Type::Label){
				goto_label(current->text);
			}
		} //else not found, current is already past instruction (no change needed)
	}
}

void Program::end_(const Args&){
	current = tokens.end();
}

void Program::goto_label(const std::string& lbl){
	for (auto itr = tokens.begin(); itr != tokens.end(); itr++){
		if (*itr == Token{lbl, Type::Label}){
			if (itr == tokens.begin() || 
			((itr-1)->type == Type::Newl)){
				current = itr;
				break;
			}
		}
	}
}

void Program::goto_(const Args& a){
	//GOTO <label>
	//GOTO <string expression>
	std::string lbl = std::get<String>(e.evaluate(a[1]));
	goto_label(lbl);
}

void Program::gosub_(const Args& a){
	//GOSUB <label expression>
	std::string lbl = std::get<String>(e.evaluate(a[1]));

	gosub_calls.push(current);	
	goto_label(lbl);
}

void Program::return_(const Args& ){
	current = gosub_calls.top();
	gosub_calls.pop();
}

void Program::on_(const Args& a){
	//ON <label exp> GOTO/GOSUB <lbl>, <lbl>, <lbl>
	int index = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	
	std::string lbl = std::get<String>(e.evaluate(a[index+3]));
	if (a[2][0].text == "GOSUB"){
		gosub_calls.push(current);
	}
	goto_label(lbl);
}

void Program::data_(const Args&){
	//DATA doesn't do anything as an instruction.
}

void Program::read_(const Args& a){
	//READ var1[,var2,var3$,...]
	
	auto expr = [](auto& current, const auto& tokens){
		auto data_end = std::min(std::find(current, tokens.end(), Token{"\r", Type::Newl}),
			std::find(current, tokens.end(), Token{",", Type::Op}));
		auto data_exp = std::vector<Token>(current, data_end);
		if (data_end->text == "\r"){
			current = std::find(data_end, tokens.end(), "DATA"_TC); //search for next DATA statement
			if (current != tokens.end())
				current++; //first piece of data will be directly after DATA statement
		} else if (data_end->text == ","){
			current = data_end+1;
		}
		return data_exp;
	};
	
	for (auto i = 1; i < (int)a.size(); ++i){
		auto value_pieces = expr(data_current, tokens);
		Token data_value{"", Type::Str};
		for (auto tok : value_pieces){
			data_value.text += tok.text;
		}
		
		auto& name_exp = a[i];
		
		e.assign(name_exp, data_value);
	}
}

void Program::clear_(const Args&){
	e.vars.clear_();
}

void Program::dim_(const Args& a){
	//DIM ARR(arg1 [,arg2]), [ARR2(arg1 [,arg2])]
	for (auto i = 1; i < (int)a.size(); ++i){
		auto p = e.process(a[i]);
		e.calculate(p, true); //boolean flag to do array creation
	}
}


std::vector<Token> tokenize(PRG& prg){
	return tokenize(prg.data.data(), prg.data.size());
}
