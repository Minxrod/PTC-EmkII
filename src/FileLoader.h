#pragma once

#include <fstream>
#include <istream>
#include <array>

struct Header {
	std::vector<unsigned char> data;
	
	int get_size() const;
	std::string get_name() const;
	std::string get_type() const;
	std::string get_md5() const;
};

std::ifstream get_filestream(std::string);
std::ofstream write_filestream(std::string);

void read_n(std::ifstream&, std::vector<unsigned char>&, int);
void write_n(std::ofstream&, std::vector<unsigned char>&);

//void read_data(std::ifstream&, std::vector<unsigned char>&);
