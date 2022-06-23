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

struct Resources{
	std::vector<std::string> chr_resources{
		"BGU0L", "BGU1L", "BGU2L", "BGU3L",
		"BGU0U", "BGU1U", "BGU2U", "BGU3U",
		"BGF0U",
		"BGF0L",
		"BGD0U", "BGD1U",
		"BGD0L", "BGD1L",
		"SPS0L", "SPS1L",
		"SPS0U", "SPS1U",
		"SPU0", "SPU1", "SPU2", "SPU3", "SPU4", "SPU5", "SPU6", "SPU7",
		"SPD0", "SPD1", "SPD2", "SPD3",
	};
	
	std::vector<std::string> col_resources{
		"COL0U", "COL1U", "COL2U",
		"COL0L", "COL1L", "COL2L",
	};
	
	std::vector<std::string> grp_resources{
		"GRP0", "GRP1", "GRP2", "GRP3"
	};
	
	std::vector<std::string> scr_resources{
		"SCU0U", "SCU1U", "SCU0L", "SCU1L"
	};

	//should contain all required resources
	Header prg_info;
	PRG prg;
	MEM mem;
	std::map<std::string, GRP> grp; //grp0-3 (ul does nothing, gpage does nothing)
	std::map<std::string, CHR> chr; //bgu0-3*2,bgf0*2,bgd0-1*2,spu0-7 (on ul),sps0-1*2,spd0-3 (on ul)
	std::map<std::string, SCR> scr;	//scu0-1*2 //works with ul and bgpage
	std::map<std::string, COL> col; //col0-2*2 //ul only
	
	Resources() = default;
	
	Resources(const Resources&) = delete;
	
	Resources& operator=(const Resources&) = delete;
	
	void load_program(std::string name);
	void load_default();
	
	void load(std::string type, std::string filename);
	void save(std::string type, std::string filename);
	
	std::string normalize_type(std::string type);
};

template <typename T>
void load_default(T& resource, std::string path){
	auto fs = get_filestream(path);
	read_n(fs, resource.data, 48); //dummy read to skip header
	read_n(fs, resource.data, CHR::SIZE);
}

