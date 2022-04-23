#pragma once

#include <array>
#include <vector>
#include <cstddef>
#include <map>
#include <utility>

#include "Vars.h"
#include "Evaluator.h"
#include "FileLoader.h"

struct PRG {
	std::vector<unsigned char> data;
};

struct CHR{
	static const int SIZE = 256*8*8/2;
	
	std::vector<unsigned char> data;
	
	CHR() = default;
	
	unsigned char get_pixel(int c, int x, int y){
		auto ch = data[32*c+x/2+4*y];
		return (x%2==0) ? (ch & 0x0f) : ((ch & 0xf0) >> 4);
	};
	
	void set_pixel(int c, int x, int y, int col){
		data[32*c+x/2+4*y] |= (x%2==0) ? col : (col << 4);
	}
	
	std::vector<unsigned char> get_array(){
		std::vector<unsigned char> array(256*8*8*4, 0);
		
		for (int i = 0; i < 256; ++i){
			int x = i % 32;
			int y = i / 32;
			for (int cx = 0; cx < 8; ++cx){
				for (int cy = 0; cy < 8; ++cy){
					array[4*(8*x+cx+256*(8*y+cy))] = get_pixel(i,cx,cy);
				}
			}
		}
		
		return array;
	};
};

struct MEM{
	static const int SIZE = 256*2;
	
	std::vector<unsigned char> data;
	
	std::string get_mem();
};

struct SCR{
	static const int SIZE = 64*64*2;
	
	std::vector<unsigned char> data;
};

struct COL{
	static const int SIZE = 256*2;
	
	std::vector<unsigned char> data;
	
	std::vector<unsigned char> COL_to_RGBA(){
		std::vector<unsigned char> cols;
		cols.resize(256*4);
		
		for (int i = 0; i < 256; ++i){
			int col = (data[2*i] << 8) + data[2*i+1];
			uint8_t red = (col & 0x1F00) >> 8;
			uint8_t green = ((col & 0xE000) >> 13) | ((col & 0x0003) << 3);
			uint8_t blue = (col & 0x007C) >> 2;
			cols[4 * i] = 8 * red;
			cols[4 * i + 1] = 8 * green;
			cols[4 * i + 2] = 8 * blue;
			cols[4 * i + 3] = (i % 16 == 0) ? 0 : 255;
		}
		return cols;
	}
};

struct GRP{
	static const int SIZE = 256*192;
	
	std::vector<unsigned char> data;
};

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
	void load_program(std::string name);
	void load_default();
};

template <typename T>
void load_default(T& resource, std::string path){
	auto fs = get_filestream(path);
	read_n(fs, resource.data, 48); //dummy read to skip header
	read_n(fs, resource.data, CHR::SIZE);
}

