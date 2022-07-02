#pragma once

#include <array>
#include <vector>
#include <cstddef>
#include <map>
#include <utility>

#include "Vars.hpp"
#include "Evaluator.hpp"
#include "FileLoader.hpp"

#include "ResourceTypes.hpp"

/// Manages all PTC resource types.
struct Resources{
	/// All valid full names for CHR resources.
	const static std::vector<std::string> chr_resources;
	/// All valid full names for COL resources.
	const static std::vector<std::string> col_resources;
	/// All valid full names for GRP resources.
	const static std::vector<std::string> grp_resources;
	/// All valid full names for SCR resources.
	const static std::vector<std::string> scr_resources;
	
	/// Current BG page
	int bgpage = 0;
	/// Current sprite page
	int sppage = 0;	
	
	//should contain all required resources
	/// Program header
	Header prg_info;
	/// PRG resource
	PRG prg;
	/// MEM resource
	MEM mem;
	/// GRP resources
	std::map<std::string, GRP> grp; //grp0-3 (ul does nothing, gpage does nothing)
	/// CHR resources
	std::map<std::string, CHR> chr; //bgu0-3*2,bgf0*2,bgd0-1*2,spu0-7 (on ul),sps0-1*2,spd0-3 (on ul)
	/// SCR resources
	std::map<std::string, SCR> scr;	//scu0-1*2 //works with ul and bgpage
	/// COL resources
	std::map<std::string, COL> col; //col0-2*2 //ul only
	
	/// Default constructor (default)
	Resources() = default;
	
	/// Copy constructor (deleted)
	Resources(const Resources&) = delete;
	
	/// Copy assignment (deleted)
	Resources& operator=(const Resources&) = delete;
	
	/// Loads a program from a file.
	/// 
	/// @param name PRG file to load
	void load_program(std::string name);
	/// Loads all default PTC resources.
	void load_default();
	
	/// Loads a resource of the given type and name.
	/// 
	/// @param type PTC resource type (MEM, BGU0L, etc.)
	/// @param filename Name of file to load
	void load(std::string type, std::string filename);
	/// Saves a resource of the given type with the given name.
	/// 
	/// @param type PTC resource type (MEM, BGU0L, etc.)
	/// @param filename Name of file to save
	void save(std::string type, std::string filename);
	
	/// Converts incomplete types to full types.
	/// Example: BGU0 -> BGU0U
	///
	/// @param type Type to convert
	/// @param bg Current BG screen
	/// @param sp Current sprite screen
	/// @return Full type
	std::string normalize_type(std::string type, int bg = 0, int sp = 0);
};

/// Function to load data into a resource type.
/// 
/// @param resource Resource object to load into
/// @param path File path to load from
template <typename T>
void load_default(T& resource, std::string path){
	auto fs = get_filestream(path);
	read_n(fs, resource.data, 48); //dummy read to skip header
	read_n(fs, resource.data, T::SIZE);
}

///TODO: save function like this one
