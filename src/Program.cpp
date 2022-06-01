#include "Program.h"

#include <thread>
#include <chrono>

Program::Program(Evaluator& eval, const std::vector<Token>& t) : e{eval}, tokens{t}{
	current = tokens.cbegin();
	data_current = std::find(tokens.begin(), tokens.end(), "DATA"_TC);
	if (data_current != tokens.end())
		data_current++; //first piece of data will be directly after DATA statement
	
	int line = 1;
	line_starts.push_back(0);
	line_starts.push_back(0);
	for (std::size_t i = 1; i < tokens.size(); ++i){
		if (tokens[i-1] == Token{"\r",Type::Newl}){
			line++;
			line_starts.push_back(i);
		}
	}
	
	for (auto i : line_starts){
		std::cout << i << std::endl;
	}
	
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
		cmd_map("RESTORE"_TC, getfunc(this, &Program::restore_)),
		cmd_map("SWAP"_TC, getfunc(this, &Program::swap_)),
		cmd_map("SORT"_TC, getfunc(this, &Program::sort_)),
		cmd_map("RSORT"_TC, getfunc(this, &Program::rsort_)),
		cmd_map("DTREAD"_TC, getfunc(&eval, &Evaluator::dtread_)),
		cmd_map("TMREAD"_TC, getfunc(&eval, &Evaluator::tmread_))
	};
}

void Program::set_breakpoint(int line, bool){
	breakpoints.insert(line_starts[line]);
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

//#include <valgrind/callgrind.h>

void Program::run_(){
//	CALLGRIND_START_INSTRUMENTATION;
//	CALLGRIND_TOGGLE_COLLECT;
	current = tokens.cbegin();
	std::cout << "\nbegin run\n" << std::endl;
	
	while (current != tokens.cend()){
		auto instr = next_instruction();
		
		auto d = std::distance(tokens.cbegin(), current);
		if (breakpoints.count(d)){
			std::cout << "Break at " << d << std::endl;
			int test;
			std::cin >> test;
		}
		
		if (instr.empty())
			continue; //it's an empty line, don't try to run it
		
		auto chunks = split(instr);
		auto instr_form = chunks[0][0]; //if chunks[0] is empty, we have other problems
		
		//std::cout << std::distance(tokens.begin(), current);
//		for (auto& chunk : chunks)
//			print("Instr:", chunk);
		
/*		if (instr_form.type == Type::Rem){ //ignore it, this is the entire line
		}else*/if (instr_form.type == Type::Label){ //ignore
		} else if (instr_form.type == Type::Num){ //error
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
	
//	CALLGRIND_TOGGLE_COLLECT;
//	CALLGRIND_STOP_INSTRUMENTATION;
}

void Program::run(){
	std::thread t{&Program::run_, this};
	t.detach();
}

bool for_continues(Number val, Number end, Number step){
	bool is_valid = true;
	if (step < 0){
		if (val < end){
			is_valid = false;
		}
	} else if (step > 0){
		if (val > end){
			is_valid = false;
		}
	} else { //same starting and ending values
	}
	return is_valid;
}

void Program::for_(const Args& a){
	//FOR <initial> TO <value> [STEP <value>]
	auto& init = a[1]; //initial
	e.evaluate(init); //run init condition

	auto eq = std::find(init.begin(), init.end(), "="_TO);
	auto var = std::vector<Token>{init.begin(), eq}; //read until = to get var expression
	
	auto& end = a[3]; //end value
	Expr step;
	if (a.size() == 6){
		step = a[5]; //step value
	} else {
		step = Expr{Token{"1", Type::Num}};
	}	
	
	Number initial_i = std::get<Number>(e.evaluate(var));
	Number final_i = std::get<Number>(e.evaluate(end));
	Number step_i = std::get<Number>(e.evaluate(step));

	if (for_continues(initial_i, final_i, step_i))
		for_calls.push_back(std::make_tuple(var, current, end, step)); //needed for looping
	else {
		//skip to NEXT
		bool has_next = false;
		do {
			auto instr = next_instruction();
			while (!instr.size() || !(instr[0] == "NEXT"_TC)){
				instr = next_instruction();
			}
			// have found a next...
			auto chunks = split(instr);
			if (chunks.size() == 1 || var == chunks[1])
				has_next = true;
		} while (!has_next);
	}
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
	
	Expr next = std::get<0>(*itr);
	next.push_back("="_TO);
	next.insert(next.end(), (std::get<0>(*itr)).begin(), (std::get<0>(*itr)).end());
	next.push_back("+"_TO);
	next.insert(next.end(), (std::get<3>(*itr)).begin(), (std::get<3>(*itr)).end());
	e.evaluate(next); //do the next step

	Number step = std::get<Number>(e.evaluate(std::get<3>(*itr))); //step amount
	Number end = std::get<Number>(e.evaluate(std::get<2>(*itr))); //end value
	Number value = std::get<Number>(e.evaluate(std::get<0>(*itr))); //current value
	
	if (for_continues(value, end, step)){
		current = std::get<1>(*itr);		
	} else {
		for_calls.erase(itr); //remove from ""stack"" when loop ends
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
	//std::cout << "GOTO @" << lbl << std::endl;
}

void Program::gosub_(const Args& a){
	//GOSUB <label expression>
	std::string lbl = std::get<String>(e.evaluate(a[1]));

	gosub_calls.push(current);	
	goto_label(lbl);
	//std::cout << "GOSUB @" << lbl << std::endl;
}

void Program::return_(const Args& ){
	current = gosub_calls.top();
	gosub_calls.pop();
	//std::cout << "RETURN" << std::endl;
}

void Program::on_(const Args& a){
	//ON <label exp> GOTO/GOSUB <lbl>, <lbl>, <lbl>
	int index = static_cast<int>(std::get<Number>(e.evaluate(a[1])));
	
	int max_index = a.size() - 3;
	
	if (index >= 0 && index < max_index){
		std::string lbl = std::get<String>(e.evaluate(a[index+3]));
		if (a[2][0].text == "GOSUB"){
			gosub_calls.push(current);
		}
		goto_label(lbl);
	}
	//std::cout << "ON " << lbl << std::endl;
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

void Program::restore_(const Args& a){
	//RESTORE label
	String lbl = std::get<String>(e.evaluate(a[1]));
	
	for (auto itr = tokens.begin(); itr != tokens.end(); itr++){
		if (*itr == Token{lbl, Type::Label}){
			if (itr == tokens.begin() || 
			((itr-1)->type == Type::Newl)){
				auto data_itr = std::find(itr, tokens.end(), "DATA"_TC);
				data_current = data_itr+1;
				break;
			}
		}
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

void Program::swap_(const Args& a){
	//SWAP var1, var2
	auto var1value = e.evaluate(a[1]);
	auto var2value = e.evaluate(a[2]);
	
	if (var1value.index() == var2value.index()){
		auto token_type = std::holds_alternative<Number>(var1value) ? Type::Num : Type::Str;
		std::string val1 = std::holds_alternative<Number>(var1value) ? std::to_string(std::get<Number>(var1value)) : std::get<String>(var1value);
		std::string val2 = std::holds_alternative<Number>(var2value) ? std::to_string(std::get<Number>(var2value)) : std::get<String>(var2value);
				
		e.assign(a[1], Token{val2, token_type});
		e.assign(a[2], Token{val1, token_type});
	} else {
		throw std::runtime_error{"Type mismatch"};
	}
}

//https://stackoverflow.com/a/17074810
template <typename T>
void apply_permutation_in_place(
	std::vector<T>& vec,
	const std::vector<std::size_t>& p)
{
	std::vector<bool> done(vec.size());
	for (std::size_t i = 0; i < vec.size(); ++i)
	{
		if (done[i])
		{
			continue;
		}
		done[i] = true;
		std::size_t prev_j = i;
		std::size_t j = p[i];
		while (i != j)
		{
			std::swap(vec[prev_j], vec[j]);
			done[j] = true;
			prev_j = j;
			j = p[j];
		}
	}
}

void Program::sort_(const Args& a){
	//SORT start, num_elems, arr1 [,arr2, ...]
	auto start = std::get<Number>(e.evaluate(a[1]));
	auto num_elems = std::get<Number>(e.evaluate(a[2]));
	//https://stackoverflow.com/questions/17074324/how-can-i-sort-two-vectors-in-the-same-way-with-criteria-that-uses-only-one-of
	std::vector<Array1*> arrays{};
	for (std::size_t i = 3; i < a.size(); ++i){
		arrays.push_back(std::get<Array1*>(e.vars.get_var_ptr(a[i][0].text+"[]"))); //stupid hack, probably isn't reliable
	}
	
	Array1* keyArray = arrays[0];
	std::vector<std::size_t> sort_array{};
	for (std::size_t i = 0; i < keyArray->size(); ++i){
		sort_array.push_back(i);
	}
	
	auto comp = [keyArray](std::size_t a, std::size_t b){ return (*keyArray)[a] < (*keyArray)[b]; };
	std::sort(sort_array.begin()+start, sort_array.begin()+start+num_elems, comp);
	
	// I realized this bit would be annoying to write so I'm copying the 
	// stackoverflow answer instead of trying to rewrite the solution
	for (auto* a : arrays){
		auto& arr = *a;
		apply_permutation_in_place(arr, sort_array);
	}
}

//TODO separate out all of this shared code
void Program::rsort_(const Args& a){
	//RSORT start, num_elems, arr1 [,arr2, ...]
	auto start = std::get<Number>(e.evaluate(a[1]));
	auto num_elems = std::get<Number>(e.evaluate(a[2]));
	//https://stackoverflow.com/questions/17074324/how-can-i-sort-two-vectors-in-the-same-way-with-criteria-that-uses-only-one-of
	std::vector<Array1*> arrays{};
	for (std::size_t i = 3; i < a.size(); ++i){
		arrays.push_back(std::get<Array1*>(e.vars.get_var_ptr(a[i][0].text+"[]"))); //stupid hack, probably isn't reliable
	}
	
	Array1* keyArray = arrays[0];
	std::vector<std::size_t> sort_array{};
	for (std::size_t i = 0; i < keyArray->size(); ++i){
		sort_array.push_back(i);
	}
	
	auto comp = [keyArray](std::size_t a, std::size_t b){ return (*keyArray)[a] > (*keyArray)[b]; };
	std::sort(sort_array.begin()+start, sort_array.begin()+start+num_elems, comp);
	
	// I realized this bit would be annoying to write so I'm copying the 
	// stackoverflow answer instead of trying to rewrite the solution
	for (auto* a : arrays){
		auto& arr = *a;
		apply_permutation_in_place(arr, sort_array);
	}
}

std::vector<Token> tokenize(PRG& prg){
	return tokenize(prg.data.data(), prg.data.size());
}
