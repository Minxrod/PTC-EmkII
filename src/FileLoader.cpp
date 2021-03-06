#include <fstream>
#include <istream>
#include <array>
#include <vector>
#include <string>

#include "FileLoader.hpp"
#include "md5.h"

const int PTC_OFS_PX01 = 0x0;
const int PTC_OFS_SIZE_AFTER_MD5 = 0x4;
const int PTC_OFS_TYPE = 0x8;
const int PTC_OFS_FILENAME = 0xc;
const int PTC_OFS_MD5 = 0x14;
const int PTC_OFS_TYPE_STR = 0x24;
const int PTC_OFS_PRG_PACKAGE_HIGH = 0x30;
const int PTC_OFS_PRG_PACKAGE = 0x34;
const int PTC_OFS_PRG_SIZE = 0x38;

const std::string PX01 = "PX01";
const std::string MD5_PREFIX = "PETITCOM";

const std::string PRG_TYPE = "PETC0300RPRG"; //0
const std::string MEM_TYPE = "PETC0200RMEM"; //1
const std::string GRP_TYPE = "PETC0100RGRP"; //2
const std::string CHR_TYPE = "PETC0100RCHR"; //3
const std::string SCR_TYPE = "PETC0100RSCR"; //4
const std::string COL_TYPE = "PETC0100RCOL"; //5


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
	} else if (type == "GRP"){
		type_str = GRP_TYPE;
		type_num = 2;
	} else if (type == "CHR"){
		type_str = CHR_TYPE;
		type_num = 3;
	} else if (type == "SCR"){
		type_str = SCR_TYPE;
		type_num = 4;
	} else if (type == "COL"){
		type_str = COL_TYPE;
		type_num = 5;
	} else {
		throw std::runtime_error{"Unsupported save type " + type};
	}
	
	write_u32(type_num, &data[PTC_OFS_TYPE]);
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
	if (data.size() != 60){
		throw std::logic_error{"Error: Attempted to get program size from non-program header"};
	}
	int size = data[PTC_OFS_PRG_SIZE] + ((unsigned int)data[PTC_OFS_PRG_SIZE+1] << 8) + ((unsigned int)data[PTC_OFS_PRG_SIZE+2] << 16);
	
	while (size % 4)
		++size;
	
	return size;
}

bool Header::is_packed(int index) const {
	if (data.size() != 60){
		throw std::logic_error{"Error: Attempted to get package information from non-program header"};
	}
	int byte_index = PTC_OFS_PRG_PACKAGE_HIGH + ((index / 8) ^ 4);
	return data[byte_index] & (1<<(index%8));
	
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
		throw std::runtime_error{"Failed to open " + file + " for reading"};
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
