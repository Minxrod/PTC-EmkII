#include "Debugger.hpp"
#include "PTCSystem.hpp"

typedef sf::Keyboard::Key Key;

void DebugConsole::draw(sf::RenderTarget& target, sf::RenderStates rs) const {
	for (int x = 0; x < get_w(); ++x){
		for (int y = 0; y < get_h(); ++y){
			tm.tile(x, y, static_cast<unsigned char>(chkchr(x,y)));
			tm.palette(x, y, 16 * get_fg(x,y), get_bg(x,y) > 0 ? 15 + 16 * get_bg(x,y) : 0);
		}
	}
	target.draw(tm, rs);
}

Debugger::DebugWindow::DebugWindow(Debugger* d, sf::Texture& t, int p) : sf::RenderWindow{sf::VideoMode(t.getSize().x, t.getSize().y), "Debugger"}, debugger{d} {
	spr.setTexture(t);
	spr.setColor(sf::Color(16*p,0,0));
}

void Debugger::DebugWindow::update(){
	sf::Event event;
	while (isOpen() && pollEvent(event))
	{
		if (event.type == sf::Event::Closed){
			close();
		}
	}
	
	clear(sf::Color(32,8,128));
	auto rs = debugger->visual->set_state(0, 0.1f);
	sf::RenderWindow::draw(spr, rs);
	display();
}


Debugger::Debugger(PTCSystem* ptc) : window{sf::VideoMode(256, 192), "Debugger"} {
	window.setSize(sf::Vector2u{512,384});
	program = ptc->get_program();
	visual = ptc->get_visual();
	resources = ptc->get_resources();
	evaluator = ptc->get_evaluator();
}

bool cmd(std::string checkfor, std::string checkin){
	return checkin.substr(0,checkfor.size()) == checkfor;
}

std::string arg(std::string command){
	return command.substr(command.find(' ')+1);
}

void Debugger::update(){
	if (!window.isOpen())
		return;
	
	sf::Event event;
	while (window.isOpen() && window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed){
			window.close();
			if (texture_window){
				texture_window->close();
			}
			return; //nothing more to be done
		}
		//TODO: maybe replace with TextEntered?
		if (event.type == sf::Event::TextEntered){
			sf::String x = event.text.unicode;
			if (x == L"\b")
				command = command.substr(0, command.size()-1);
			else if (x == L"\r")
				submit = true;
			else
				command += event.text.unicode;
		}
	}
	
	// Actions
	if (submit){
		// creating this command in particular made me realize this is pretty much how easy it would be to implement DIRECT mode
		// just instead of calling evaluate, create/update a Program object and run that
		if (cmd("tr", command)){
			PRG prg;
			for (auto c : arg(command))
				prg.data.push_back(c);
			
			expression.push_back(arg(command));
			evaluable.push_back(tokenize(prg));
//			print("Tokenized",evaluable.back());
		} else if (cmd("rm", command)){
			auto itr = std::find(expression.begin(), expression.end(), arg(command));
			if (itr != expression.end()){
				auto dist = std::distance(expression.begin(), itr);
				evaluable.erase(evaluable.begin() + dist);
				expression.erase(expression.begin() + dist);
			} else {
				feedback = "Expression does not exist";
			}
		} else if (cmd("br", command) || cmd("clear", command)){
			try {
				program->set_breakpoint(std::stoi(arg(command)), cmd("br", command));
			} catch (std::invalid_argument& e){
				feedback = "Error: Invalid argument";
			}
		} else if (cmd("c", command)){
			program->pause(false);
		} else if (cmd("close", command) || cmd("exit", command)){
			window.close();
			return;
		} else if (cmd("tex", command)){
			try {
				auto index = std::stoi(arg(command));
				if (index == 4 || index == 8){
					throw std::out_of_range{"Undefined texture"};
				}
				auto color = 0;
				try {
					color = std::stoi(arg(arg(command+" 0")));
				} catch (std::invalid_argument& e){
					// use default
					color = 0;
				}
				auto& tex = visual->resource_tex.at(index);
				texture_window = std::make_unique<DebugWindow>(this, tex, color);
			} catch (std::out_of_range& e){
				feedback = "Invalid texture index";
			} catch (std::invalid_argument& e){
				feedback = "Invalid texture index";
			}
		} else {
			feedback = "Unrecognized command";
			/*
			PRG prg;
			for (auto c : command.substr(3))
				prg.data.push_back(c);
			evaluator->evaluate(tokenize(prg));
			*/
			// This idea requires some cross-thread communication to be in-place to be safe (not yet implemented)
			// This is essentially a limited DIRECT mode, in a separate window.
			// Idea II: If Program is made much nicer across threads, this can just execute anything.
		}
		
		submit = false;
		command = "";
	}
	
	// Render (console)
	console.cls();
	console.color(0);
	console.print("Debug console");
	console.print(command);
	
	// Variables
	console.locate(0,3);
	console.print("Expressions:");
	for (std::size_t i = 0; i < evaluable.size(); ++i){
		auto var = evaluator->evaluate(evaluable.at(i));
		auto prefix = String(expression.at(i).cbegin(), expression.at(i).cend()) + L"=";
		if (std::holds_alternative<Number>(var)){
			console.print(to_string(prefix + std::to_wstring(std::get<Number>(var))));
		} else {
			console.print(to_string(prefix + std::get<String>(var)));
		}
	}
	
	// Breakpoints
	console.print("");
	console.print("Breakpoints:");
	for (auto l : program->breakpoints){
		console.print(std::to_string(l));
	}
	
	// Errors from debugger
	console.locate(0,23);
	console.color(13);
	console.print(feedback, false);
	
	// Render (actual)
	window.clear();
	
	//borrow some stuff from visual because why not (note: will cause problems eventually)
	//TODO: Generate a texture and just use that for everything
	visual->bgsp_shader.setUniform("colors", visual->col_tex);
	visual->bgsp_shader.setUniform("texture", sf::Shader::CurrentTexture);
	sf::RenderStates rs;
	rs.shader = &visual->bgsp_shader;
	rs.texture = &visual->resource_tex[0];
	
	window.draw(console, rs);
	
	window.display();
	
	if (texture_window){
		texture_window->update();
	}
}
