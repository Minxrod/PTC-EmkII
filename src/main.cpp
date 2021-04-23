#include <SFML/Graphics.hpp>

#include "Vars.h"
#include "Evaluator.h"
#include "FileLoader.h"
#include "Resources.h"

#include <iostream>

int main()
{
    //sf::RenderWindow window(sf::VideoMode(256, 384), "PTCEmukII");
    
    const char* string = "hello\0";
    String str{ string };
    std::cout << str << std::endl;

    Number num{ 0x33 };
    std::cout << num << std::endl;
	
	Evaluator e{};
	
	/*std::vector<Token> 	exp{Token{"2",Type::Num},
							Token{"+",Type::Op},
							Token{"3",Type::Num},
							Token{"*",Type::Op},
							Token{"5",Type::Num}};
	*/
	
	/*std::vector<Token> 	exp{Token{"5",Type::Num},
							Token{"*",Type::Op},
							//"!"_TO,
							Token{"(",Type::Op},
							Token{"2",Type::Num},
							Token{"+",Type::Op},
							Token{"3",Type::Num},
							Token{")",Type::Op},
							};
	*/
	
	/*std::vector<Token> exp {Token{"7",Type::Num},
							"*"_TO, Token{"FNC",Type::Func}, "("_TO,
							Token{"5",Type::Num}, "+"_TO, Token{"7.5", Type::Num}, ","_TO, Token{"5.5",Type::Num}, 								","_TO, Token{"75",Type::Num}, ")"_TO};*/
	
	/*std::vector<Token> exp {Token{"6",Type::Num},
							"AND"_TO,
							Token{"5",Type::Num}};*/

	std::vector<Token> exp {"COS"_TF, "("_TO, Token{"PIE", Type::Var}, ")"_TO};

	auto r = e.process(exp);
	
	print("RESULT", r);

	auto v = e.calculate(r);

	std::cout << std::get<Number>(v) << std::endl;
	
	//PROGRAM/TOKENIZATION TESTS
	Header h{};
	
	PRG p{};
	auto fs = get_filestream("programs/SAMPLE7.PTC");
	read_n(fs, h.data, 60);
	read_n(fs, p.data, h.get_size());
	
	auto tk = tokenize(p);
	
	Program program(tk);
	
	int n = 0;
	while(!program.at_eof()){
		auto tok = program.next_instruction();
		auto proc = e.split(tok);
		for (uint32_t p = 0; p < proc.size(); ++p){ 
			print("V"+std::to_string(p), proc[p]);
		}
		
		n++;
		print("Instr " + std::to_string(n), tok);
	}
	
	//DO THIS NEXT: implement some sort of program class to
	//read lines /yes
	//execute commands
	//call evaluator
	
	//print("TOKENIZED:", tk);
	
    /*while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        //get updated textures for drawing to screen
        
        
        
        window.clear();

        //draw textures



        window.display();
    }*/

    return 0;
}
