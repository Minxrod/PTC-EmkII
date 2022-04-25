#include <fstream>
#include <istream>
#include <array>
#include <vector>
#include <string>

#include "FileLoader.h"

int Header::get_size() const {
	return data[0x38] + ((unsigned int)data[0x39] << 8) + ((unsigned int)data[0x3a] << 16);
}

std::string Header::get_md5() const {
	//todo: proper conversion to string md5
	return std::string(reinterpret_cast<const char*>(&data[0x14]), 12);
}

std::string Header::get_name() const {
	return std::string(reinterpret_cast<const char*>(&data[0x0c]), 8);
}

std::string Header::get_type() const {
	return std::string(reinterpret_cast<const char*>(&data[0x24]), 12);
}

std::ifstream get_filestream(std::string file){
	std::ifstream ifs{file, std::ios::binary | std::ios::in};
	if (!ifs)
		throw std::runtime_error{file};
	return ifs;
}

std::ofstream write_filestream(std::string file){
	std::ofstream ofs{file, std::ios::binary | std::ios::out};
	if (!ofs)
		throw std::runtime_error{file};
	return ofs;
}


void read_n(std::ifstream& ifs, std::vector<unsigned char>& data, int n){
	data.resize(n);
	ifs.read(reinterpret_cast<char*>(data.data()), n);
}

void write_n(std::ofstream& ofs, std::vector<unsigned char>& data){
	int n = data.size();
	ofs.write(reinterpret_cast<char*>(data.data()), n);
}
