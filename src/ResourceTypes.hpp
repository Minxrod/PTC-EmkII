#pragma once

#include <vector>
#include <string>
#include <map>

/// PRG resource class. Stores the actual data of a PRG file.
struct PRG {
	/// Data as read from a PRG file.
	std::vector<unsigned char> data;
	
	/// Loads data from a file.
	/// @param filename PRG File to load
	void load(std::string filename);
};

/// CHR resource class. Stores the data of a CHR file.
struct CHR{
	/// Size of a CHR file (bytes)
	static const int SIZE = 256*8*8/2;
	
	/// Data as read from a CHR file.
	std::vector<unsigned char> data;
	
	/// Default constructor
	CHR() = default;
	
	/// Gets a pixel from within a character.
	/// 
	/// @param c Character code [0-255]
	/// @param x x coordiante within character [0-7]
	/// @param y y coordinate within character [0-7]
	/// @return Color index [0-15]
	unsigned char get_pixel(int c, int x, int y);
	
	/// Sets a pixel within a character.
	/// @param c Character code [0-255]
	/// @param x x coordiante within character [0-7]
	/// @param y y coordinate within character [0-7]
	/// @param col Color index [0-15]
	void set_pixel(int c, int x, int y, int col);
	
	/// Returns an array used to generate the texture for rendering.
	/// @return Texture data array
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

