#include <fstream>
#include <istream>
#include <array>
#include <vector>
#include <string>

#include "FileLoader.h"
#include "md5.h"

const int PTC_OFS_PX01 = 0x0;
const int PTC_OFS_SIZE_AFTER_MD5 = 0x4;
const int PTC_OFS_UNKNOWN = 0x8; //might be a type identifier of some kind? 0x00 - PRG, 0x01 - MEM, 0x03 - CHR, 0x05 - COL?
const int PTC_OFS_FILENAME = 0xc;
const int PTC_OFS_MD5 = 0x14;
const int PTC_OFS_TYPE_STR = 0x24;
const int PTC_OFS_PRG_UNKNOWN = 0x30; //possibly extended package info or something
const int PTC_OFS_PRG_PACKAGE = 0x34;
const int PTC_OFS_PRG_SIZE = 0x38;

const std::string PX01 = "PX01";
const std::string MD5_PREFIX = "PETITCOM";
const std::string MEM_TYPE = "PETC0200RMEM";

void write_u32(int n, unsigned char* data){
	data[0] = n & 0xff;
	data[1] = (n >> 8) & 0xff;
	data[2] = (n >> 16) & 0xff;
	data[3] = (n >> 24) & 0xff;
}

void write_str(std::size_t max_chars, std::string str, unsigned char* data){
	for (std::size_t i = 0; i < max_chars; ++i){
		data[i] = i >= str.size() ? '\0' : str[i];
	}
}

Header::Header(){
	data.resize(48); //size of header data for most PTC files
	write_str(4, PX01, &data[PTC_OFS_PX01]);
}

void Header::set_size(int n){
	write_u32(n, &data[PTC_OFS_SIZE_AFTER_MD5]);
}

void Header::set_filename(std::string filename){
	write_str(8, filename, &data[PTC_OFS_FILENAME]);
}

void Header::set_type(std::string type){
	int type_num;
	std::string type_str;
	if (type == "MEM"){
		type_str = MEM_TYPE;
		type_num = 1;
	} else {
		throw std::runtime_error{"Unsupported save type " + type};
	}
	
	write_u32(type_num, &data[PTC_OFS_UNKNOWN]);
	write_str(12, type_str, &data[PTC_OFS_TYPE_STR]);
}

void Header::set_md5(std::vector<unsigned char>& d){
	MD5 hash;
	hash.add(&MD5_PREFIX, 8);
	hash.add(&d[PTC_OFS_TYPE_STR], 12);
	hash.add(d.data(), d.size());
	hash.getHash(&data[PTC_OFS_MD5]);
}

int Header::get_prg_size() const {
	return data[0x38] + ((unsigned int)data[0x39] << 8) + ((unsigned int)data[0x3a] << 16);
}

std::string Header::get_md5() const {
	//todo: proper conversion to string md5
	MD5 md5;
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
		throw std::runtime_error{"Failed to open " + file + " for writing"};
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
