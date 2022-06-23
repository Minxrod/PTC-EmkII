#pragma once

#include <vector>
#include <string>
#include <map>

struct PRG {
	std::vector<unsigned char> data;
	
	void load(std::string filename);
};

struct CHR{
	static const int SIZE = 256*8*8/2;
	
	std::vector<unsigned char> data;
	
	CHR() = default;
	
	unsigned char get_pixel(int c, int x, int y);
	
	void set_pixel(int c, int x, int y, int col);
	
	std::vector<unsigned char> get_array();
};

struct MEM{
	static const int SIZE = 256*2;
	
	int actual_size = 0;
	
	std::vector<unsigned char> data;
	std::vector<unsigned char> data_enc;
	
	std::map<int, int> encode;
	
	void generate_encoding();
	std::string get_mem();
	void set_mem(std::string);
};

struct SCR{
	static const int SIZE = 64*64*2;
	
	std::vector<unsigned char> data;
};

struct COL{
	static const int SIZE = 256*2;
	
	COL() = default;
	
	std::vector<unsigned char> data;
	
	int RGB_to_COL(int r, int g, int b);
	
	void set_col(int i, int r, int g, int b);
	void set_col(int i, int c);
	
	int get_col_r(int i);
	int get_col_g(int i);
	int get_col_b(int i);
	
	int get_col(int i);
	
	std::vector<unsigned char> COL_to_RGBA();
	
	void load(std::string filename);
};

struct GRP{
	static const int SIZE = 256*192;
	
	std::vector<unsigned char> data;
};

