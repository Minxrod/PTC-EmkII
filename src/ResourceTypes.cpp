#include "ResourceTypes.hpp"
#include "FileLoader.hpp"

void PRG::load(std::string name){
	auto fs = get_filestream(name);
	Header prg_info;
	read_n(fs, prg_info.data, 60);
	read_n(fs, data, prg_info.get_prg_size());
}

void COL::load(std::string name){
	auto fs = get_filestream(name);
	Header col_info;
	read_n(fs, col_info.data, 48);
	read_n(fs, data, COL::SIZE);
}

unsigned char CHR::get_pixel(int c, int x, int y){
	auto ch = data[32*c+x/2+4*y];
	return (x%2==0) ? (ch & 0x0f) : ((ch & 0xf0) >> 4);
}

void CHR::set_pixel(int c, int x, int y, int col){
	data[32*c+x/2+4*y] &= (x%2==0) ? 0xf0 : 0x0f;
	data[32*c+x/2+4*y] |= (x%2==0) ? col : (col << 4);
}

//Creates the array that will be used to update the resource's texture
std::vector<unsigned char> CHR::get_array(){
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
}


std::string MEM::get_mem(){
	std::string mem{};
	for (int i = 0; i < MEM::STRING_SIZE_MAX; i+=2){
		int value = data[i] + (data[i+1]<<8);
		if (encode[value])
			mem += (char)encode[value];
		else {
			actual_size = i/2; //TODO: check if file size matches actual string size 
			break;
		}
	}
	return mem;
}

void MEM::set_mem(std::string mem){
	actual_size = mem.size();
	data.resize(MEM::SIZE);
	std::fill(data.begin(), data.end(), 0);
	for (int i = 0; i < (int)mem.size(); ++i){
		data[2*i] = data_enc[2*mem[i]];
		data[2*i+1] = data_enc[2*mem[i]+1];
	}
	//Write string size to MEM file
	data[MEM::SIZE-4] = actual_size % 256;
	data[MEM::SIZE-3] = actual_size / 256;
	data[MEM::SIZE-2] = 0;
	data[MEM::SIZE-1] = 0;
}

// requires data_enc to be loaded before using
void MEM::generate_encoding(){
	for (int i = 0; i < MEM::STRING_SIZE_MAX; i+=2){
		int value = data_enc[i] + (data_enc[i+1]<<8);
		encode[value] = i/2;
	}
}

int COL::RGB_to_COL(int r, int g, int b){
	int r5 = r >> 3;
	int g5 = g >> 3;
	int g_low = (g & 0x04) >> 2;
	int b5 = b >> 3;
	int col = (g_low << 15) + (b5 << 10) + (g5 << 5) + r5;
	return col;
}

void COL::set_col(int i, int r, int g, int b){
	set_col(i, RGB_to_COL(r,g,b));
}

void COL::set_col(int i, int c){
	data[2*i] = c & 0x00ff;
	data[2*i+1] = (c & 0xff00) >> 8;
}

//File format:
// GGGRRRRR gBBBBBGG
//As u16: (g=low bit of green, never used in default COL files)
// gBBBBBGGGGGRRRRR
int COL::get_col(int i){
	return data[2*i] + (data[2*i+1] << 8);
}

// gBBB.BBGG.GGGR.RRRR -> RRRR.R000	
int COL::get_col_r(int i){
	return (get_col(i) & 0x001F) << 3;
}

// gBBB.BBGG.GGGR.RRRR -> GGGG.Gg00
int COL::get_col_g(int i){
	return ((get_col(i) & 0x03E0) >> 2) | ((get_col(i) & 0x8000) >> 13);
}
// gBBB.BBGG.GGGR.RRRR -> BBBB.B000	
int COL::get_col_b(int i){
	return (get_col(i) & 0x7C00) >> 7;
}

//Used to create/update color texture
std::vector<unsigned char> COL::COL_to_RGBA(){
	std::vector<unsigned char> cols;
	cols.resize(1024);
	
	for (int i = 0; i < 256; ++i){
		int col = (data[2*i] << 8) + data[2*i+1];
		uint8_t red = (col & 0x1F00) >> 8;
		uint8_t green = ((col & 0xE000) >> 13) | ((col & 0x0003) << 3);
		uint8_t blue = (col & 0x007C) >> 2;
		cols[4 * i] = 8 * red;
		cols[4 * i + 1] = 8 * green;
		cols[4 * i + 2] = 8 * blue;
		cols[4 * i + 3] = (i == 0) ? 0 : 255;
	}
	return cols;
}



