#pragma once

#include "Vars.hpp"

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

/// MEM resource class. Stores the data of a MEM file.
struct MEM{
	/// Size of a MEM file, in bytes
	static const int SIZE = 256*2+4;
	/// Maximum size of the actual string
	static const int STRING_SIZE_MAX = 256*2;
	
	/// Size of actual MEM$ string contained
	int actual_size = 0;
	
	/// Actual data of MEM file
	std::vector<unsigned char> data;
	/// Vector to hold MCHRENC, used to generate encoding
	std::vector<unsigned char> data_enc;
	/// UCS-2 encoding information for MEM file
	std::map<int, int> encode;
	
	/// Generates MEM encoding for correct saving/loading
	void generate_encoding();
	/// Gets the value of this MEM object as a string
	/// @return MEM$
	String get_mem();
	/// Sets the value of this MEM object from a string
	/// @param mem New MEM$ to set
	void set_mem(String mem);
};

/// SCR resource class. Stores the data of a SCR file.
struct SCR{
	/// Size of a SCR file, in bytes
	static const int SIZE = 64*64*2;
	
	/// Actual data of SCR file
	std::vector<unsigned char> data;
};

/// COL resource class. Stores the data of a COL file.
struct COL{
	/// Size of a COL file, in bytes
	static const int SIZE = 256*2;
	
	/// Default constructo
	COL() = default;
	
	/// Actual data of COL fie
	std::vector<unsigned char> data;
	
	/// Converts RGB888 values to RGB565 COL fomat
	/// @param r Red component
	/// @param g Green component
	/// @param b Blue component
	/// @return RGB565 color value	
	int RGB_to_COL(int r, int g, int b);
	
	/// Sets from RGB888 values the color at the given index
	/// @param i Color index
	/// @param r Red component
	/// @param g Green component
	/// @param b Blue component
	void set_col(int i, int r, int g, int b);
	/// Sets from a RGB565 value the color at the given index
	/// @param i Color index
	/// @param c Color data
	void set_col(int i, int c);
	
	/// Gets the red component of a color
	/// @param i Color index
	/// @return Red component
	int get_col_r(int i);
	/// Gets the green component of a color
	/// @param i Color index
	/// @return Green component
	int get_col_g(int i);
	/// Gets the blue component of a color
	/// @param i Color index
	/// @return Blue component
	int get_col_b(int i);
	
	/// Gets the RGB565 value of a colors
	/// @param i Color index
	/// @return RGB565 value
	int get_col(int i);
	
	/// Converts this COL object into a texture data array
	/// @return Texture data array
	std::vector<unsigned char> COL_to_RGBA();
	
	/// Loads a COL file
	/// @param filename COL file to load
	void load(std::string filename);
};

/// GRP resource class. Stores the data of a GRP file.
struct GRP{
	/// Size of a GRP file, in bytes.
	static const int SIZE = 256*192;
	
	/// Actual data of GRP file
	std::vector<unsigned char> data;
};

