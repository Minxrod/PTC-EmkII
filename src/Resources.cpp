#include <string>
#include <sstream>
#include <vector>
#include <regex>
#include <algorithm>
#include <cmath>
#include <iterator>

#include <iostream>

#include "Resources.hpp"
#include "Vars.hpp"
#include "Evaluator.hpp"
#include "FileLoader.hpp"

const std::vector<std::string> Resources::chr_resources{
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

const std::vector<std::string> Resources::col_resources{
	"COL0U", "COL1U", "COL2U",
	"COL0L", "COL1L", "COL2L",
};

const std::vector<std::string> Resources::grp_resources{
	"GRP0", "GRP1", "GRP2", "GRP3"
};

const std::vector<std::string> Resources::scr_resources{
	"SCU0U", "SCU1U", "SCU0L", "SCU1L"
};

const std::vector<std::string> spk_files{
	"resources/ui/partsSPDK.NCGR",
	"resources/ui/makeALPHA_SHIFT.NCGR",
	"resources/ui/makeKIGOU.NCGR",
	"resources/ui/makeKIGOU_SHIFT.NCGR",
	"resources/ui/makeKANA.NCGR",
	"resources/ui/makeKANA_SHIFT.NCGR"
};

// this ordering taken from the program post here:
// https://gamefaqs.gamespot.com/boards/663843-petit-computer/69602216
// credit to @NeatNit for doing the research
const std::vector<std::string> package_resources{
	"SPU0", "SPU1", "SPU2", "SPU3", "SPU4", "SPU5", "SPU6", "SPU7",
	"BGU0U", "BGU1U", "BGU2U", "BGU3U",
	"BGF0U",
	"COL0U", "COL1U", "COL2U",
	"SCU0U", "SCU1U",
	"GRP0", "GRP1", "GRP2", "GRP3",
	"MEM",
	"SPD0", "SPD1", "SPD2", "SPD3",
	"BGU0L", "BGU1L", "BGU2L", "BGU3L",
	"BGF0L",
	"COL0L", "COL1L", "COL2L",
	"SCU0L", "SCU1L",
	"SPS0U", "SPS1U",
	"BGD0U", "BGD1U",
	"SPS0L", "SPS1L",
	"BGD0L", "BGD1L",
};

void load_pkg(std::ifstream& fs, std::vector<unsigned char>& data, int size){
	read_n(fs, data, 12);
	read_n(fs, data, size);
}

void Resources::load_program(std::string name){
	auto fs = get_filestream(name);
	read_n(fs, header.data, 60);
	read_n(fs, prg.data, header.get_prg_size());
	
	auto load_type_pkg = [this](std::ifstream& fs, std::string type){
		if (std::find(chr_resources.begin(), chr_resources.end(), type) != chr_resources.end()){
			load_pkg(fs, chr.at(type).data, CHR::SIZE);
		} else if (std::find(col_resources.begin(), col_resources.end(), type) != col_resources.end()){
			load_pkg(fs, col.at(type).data, COL::SIZE);
		} else if (std::find(grp_resources.begin(), grp_resources.end(), type) != grp_resources.end()){
			load_pkg(fs, grp.at(type).data, GRP::SIZE);
		} else if (std::find(scr_resources.begin(), scr_resources.end(), type) != scr_resources.end()){
			load_pkg(fs, scr.at(type).data, SCR::SIZE);
		} else if (type == "MEM"){
			load_pkg(fs, mem.data, MEM::SIZE);
		}
	};
	
	for (std::size_t r = 0; r < package_resources.size(); ++r){
		if (header.is_packed(r))
			load_type_pkg(fs, package_resources[r]);
	}
}

Resources::Resources() : header{}, prg{}, mem{}, grp{}, chr{}, scr{}, col{}{
	for (auto r : chr_resources)
		chr.insert(std::pair(r, CHR()));
	for (int i = 0; i < 4; ++i)
		chr.insert(std::pair("SPK" + std::to_string(i), CHR()));
	for (auto c : col_resources)
		col.insert({c, COL()});
	for (auto g : grp_resources)
		grp.insert(std::pair(g, GRP()));
	for (auto s : scr_resources)
		scr.insert(std::pair(s, SCR()));
	scr.insert(std::pair("PNL", SCR()));
	scr.insert(std::pair("KEY", SCR()));
	
	//Panel SCRs
	scr.insert(std::pair("PNL", SCR()));
	auto pnlfs = get_filestream("resources/ui/pnlPANEL.NSCR");
	read_n(pnlfs, scr.at("PNL").data, 36); //dummy read to skip header
	read_n(pnlfs, scr.at("PNL").data, 2*32*24);
	
	scr.insert(std::pair("KEY", SCR()));
	auto keyfs = get_filestream("resources/ui/pnlKEY.NSCR");
	read_n(keyfs, scr.at("KEY").data, 36); //dummy read to skip header
	read_n(keyfs, scr.at("KEY").data, 2*32*24);
	
	auto memfs = get_filestream("resources/misc/MCHRENC.PTC");
	read_n(memfs, mem.data_enc, 48);
	read_n(memfs, mem.data_enc, MEM::SIZE);
	mem.generate_encoding();
	
	{
		auto fs = get_filestream("resources/ui/partsSPDK.NCGR");
		std::vector<unsigned char> dummy{};
		read_n(fs, dummy, 48+4*(256*64/2)); //dummy read to skip header, SPD0-3
		for (int i = 0; i < 4; ++i){
			read_n(fs, chr.at("SPK" + std::to_string(i)).data, CHR::SIZE);
		}
	}
	
//	load_default();
}

void Resources::load_default(){
	for (auto r : chr_resources){
		auto fs = get_filestream("resources/graphics/"+r.substr(0,4)+".PTC");
		read_n(fs, chr.at(r).data, 48); //dummy read to skip header
		read_n(fs, chr.at(r).data, CHR::SIZE);
	}
	//Keyboard sprites
	{
		auto fs = get_filestream("resources/ui/partsSPDK.NCGR");
		std::vector<unsigned char> dummy{};
		read_n(fs, dummy, 48+4*(256*64/2)); //dummy read to skip header, SPD0-3
		for (int i = 0; i < 4; ++i){
			chr.insert(std::pair("SPK" + std::to_string(i), CHR()));
			read_n(fs, chr.at("SPK" + std::to_string(i)).data, CHR::SIZE);
		}
	}
	
	for (auto c : col_resources){
		col.at(c).load("resources/graphics/"+c.substr(0,4)+".PTC");
	}
	
	for (auto g : grp_resources){
		grp.at(g).data = std::vector<unsigned char>(GRP::SIZE, 0);
	}
	
	for (auto s : scr_resources){
		scr.at(s).data = std::vector<unsigned char>(SCR::SIZE, 0);
	}
}

void Resources::load_keyboard(int type){
	auto fs = get_filestream(spk_files.at(type));
	std::vector<unsigned char> dummy{};
	read_n(fs, dummy, 48); //dummy read to skip NCGR header
	if (!type){
		read_n(fs, dummy, 6*CHR::SIZE); //skip to part we care about
	}
	read_n(fs, chr.at("SPK2").data, CHR::SIZE);
	read_n(fs, chr.at("SPK3").data, CHR::SIZE);
}

std::string Resources::normalize_type(std::string type, int bg, int sp, int gp){
	if (type == "MEM")
		return type;
	
	if (type.size()==3){
		type += "0"; //assumed
	}
	if (type.size()==4){
		if(type[3] == 'L' || type[3] == 'U'){
			return type.substr(0,3) + "0" + type[3];
		}
		auto t = type.substr(0,3);
		if (t == "BGF" || t == "BGU" || t == "BGD" || type == "COL0" || t == "SCU")
			type += bg ? "L" : "U";
		else if (t == "SPS" || type == "COL1")
			type += sp ? "L" : "U";
		else if (type == "COL2")
			type += gp ? "L" : "U";
	}
	return type;
}

void Resources::load(std::string type, std::string filename, int header_size){
	if (std::find(chr_resources.begin(), chr_resources.end(), type) != chr_resources.end()){
		auto fs = get_filestream("programs/"+filename+".PTC");
		read_n(fs, chr.at(type).data, header_size); //dummy read to skip header
		read_n(fs, chr.at(type).data, CHR::SIZE);
	} else if (std::find(col_resources.begin(), col_resources.end(), type) != col_resources.end()){
		auto fs = get_filestream("programs/"+filename+".PTC");
		read_n(fs, col.at(type).data, header_size); //dummy read to skip header
		read_n(fs, col.at(type).data, COL::SIZE);
	} else if (std::find(grp_resources.begin(), grp_resources.end(), type) != grp_resources.end()){
		auto fs = get_filestream("programs/"+filename+".PTC");
		read_n(fs, grp.at(type).data, header_size); //dummy read to skip header
		read_n(fs, grp.at(type).data, GRP::SIZE);
	} else if (std::find(scr_resources.begin(), scr_resources.end(), type) != scr_resources.end()){
		auto fs = get_filestream("programs/"+filename+".PTC");
		read_n(fs, scr.at(type).data, header_size); //dummy read to skip header
		read_n(fs, scr.at(type).data, SCR::SIZE);
	} else if (type == "MEM"){
		try {
			auto fs = get_filestream("programs/"+filename+".PTC");
			read_n(fs, mem.data, header_size); //dummy read to skip header
			read_n(fs, mem.data, MEM::SIZE);
		} catch (std::runtime_error& e){
			std::cout << "Failed to load: " << e.what() << "\n";
			mem.data.resize(MEM::SIZE);
			std::fill(mem.data.begin(), mem.data.end(), 0);
			throw e;
		}
	}
}

void Resources::save(std::string type, std::string filename){
	Header header{};
	header.set_filename(filename);
	type = normalize_type(type);
	std::vector<unsigned char> data;
	
	if (type == "MEM"){
		header.set_size(12 + MEM::SIZE);
		header.set_type("MEM");
		data = mem.data;
		header.set_md5(data);
	} else if (std::find(chr_resources.begin(), chr_resources.end(), type) != chr_resources.end()){
		header.set_size(12 + CHR::SIZE);
		header.set_type("CHR");
		data = chr.at(type).data;
		header.set_md5(data);
	} else if (std::find(col_resources.begin(), col_resources.end(), type) != col_resources.end()){
		header.set_size(12 + COL::SIZE);
		header.set_type("COL");
		data = col.at(type).data;
		header.set_md5(data);
	} else if (std::find(grp_resources.begin(), grp_resources.end(), type) != grp_resources.end()){
		header.set_size(12 + GRP::SIZE);
		header.set_type("GRP");
		data = grp.at(type).data;
		header.set_md5(data);
	} else if (std::find(scr_resources.begin(), scr_resources.end(), type) != scr_resources.end()){
		header.set_size(12 + SCR::SIZE);
		header.set_type("SCR");
		data = scr.at(type).data;
		header.set_md5(data);
	}
	
	auto fs = write_filestream("programs/"+filename+".PTC");
	write_n(fs, header.data);
	write_n(fs, data);
}

