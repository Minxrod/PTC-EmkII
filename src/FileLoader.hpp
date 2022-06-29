#pragma once

#include <fstream>
#include <istream>
#include <array>

/// Header struct, containing the data of a PTC SD file header.
/// 
/// Can be used to either read from a header or create a new header.
struct Header {
	/// Actual header data (either 48 or 60 bytes in length)
	std::vector<unsigned char> data;
	
	/// Sets the file size past the MD5 hash.
	/// 
	/// @param n File size (after md5)
	void set_size(int n);
	
	/// Sets the internal file name.
	/// 
	/// @param name File name [8 characters max]
	void set_filename(std::string name);
	
	/// Sets the internal file type.
	/// @note Only MEM is implemented right now.
	/// 
	/// @param type PTC resource type (ex. MEM, PRG, CHR)
	void set_type(std::string type);
	
	/// Calculates and sets the MD5 has from provided data.
	/// 
	/// @param d Data used for MD5 calculation
	void set_md5(std::vector<unsigned char>& d);
	
	/// Gets the program size (specific to program type)
	/// 
	/// @return Program size (bytes)
	int get_prg_size() const;
	
	/// Gets the internal name.
	///
	/// @return Internal file name
	std::string get_name() const;
	
	/// Gets the internal type.
	/// 
	/// @return Internal type string
	std::string get_type() const;
	
	/// Gets the MD5 hash.
	/// @note Currently does not format as a string; read as bytes contained in a string.
	/// 
	/// @return MD5 hash.
	std::string get_md5() const;
	
	/// Default constructor
	Header();
};

std::ifstream get_filestream(std::string);
std::ofstream write_filestream(std::string);

void read_n(std::ifstream&, std::vector<unsigned char>&, int);
void write_n(std::ofstream&, std::vector<unsigned char>&);

//void read_data(std::ifstream&, std::vector<unsigned char>&);
