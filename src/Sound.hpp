#pragma once

#include <SFML/Audio.hpp>
#include <SSEQStream.h>

#include <map>
#include <vector>
#include <memory>

#include "Vars.hpp"
#include "IPTCObject.hpp"
#include "Evaluator.hpp"

class PTCSystem;

/// Manages the sound effects and background music. (BGM, BEEP)
class Sound : public IPTCObject {
	/// Max number of BGM tracks
	const static int BGM_TRACK_COUNT = 8;
	/// Max number of sound effects
	const static int SFX_COUNT = 16;
	/// Offset into SSEQ files where BGM ends and SFX begins
	const static int SEQ_SFX_OFFSET = 30;
	
	/// Is the sound system enabled?
	bool enabled = true;
	
	/// Containing system
	PTCSystem* system;
	/// Evaluator object
	Evaluator& e;
	
	/// Waveforms
	SWAR swar;
	/// Instrument definitions
	SBNK sbnk;
	/// List of sequences (both music and SFX)
	std::vector<SSEQ> sseq;
	/// User-defined songs [128-256]
	std::vector<SSEQ> user_songs; 
	
	/// List of BGM tracks
	std::vector<std::unique_ptr<SSEQStream>> bgm;
	/// List of sound effects
	std::vector<std::unique_ptr<SSEQStream>> sfx;
	
	/// Searches for the first unused SFX item.
	int get_available_sound();
	
	void beep_(const Args&);

	void bgmplay_(const Args&);
	void bgmstop_(const Args&);
	void bgmvol_(const Args&);
	void bgmclear_(const Args&);
	void bgmset_(const Args&);
	void bgmsetd_(const Args&);
	void bgmprg_(const Args&);
	void bgmsetv_(const Args&);
	
	Var bgmchk_(const Vals&);
	Var bgmgetv_(const Args&);

public:
	/// Constructor
	/// 
	/// @param s Containing system
	Sound(PTCSystem* s);
	
	/// Default constructor (deleted)
	Sound() = delete;
	
	/// Copy constructor (deleted)
	Sound(const Sound&) = delete;
	
	/// Copy assignment (deleted)
	Sound& operator=(const Sound&) = delete;
	
	std::map<Token, cmd_type> get_cmds() override;
	std::map<Token, op_func> get_funcs() override;
	
	/// Enable or disable sound system (at runtime)
	/// 
	/// @param state [true=enabled, false=disabled]
	void _enable(bool state) { enabled = state; }
};
