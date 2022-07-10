#pragma once

#include <SFML/Graphics.hpp>

#include "Vars.hpp"
#include "Resources.hpp"
#include "Evaluator.hpp"
#include "Input.hpp"

#include "PTC2Console.hpp"
#include "Background.hpp"
#include "Sprites.hpp"
#include "Graphics.hpp"
#include "Panel.hpp"
#include "IPTCObject.hpp"

/// This class is responsible for managing all components that are on screen,
/// and the texturing/colors of these components.
/// 
/// This includes stuff like the console, BG layers, sprites, and so on.
/// This class also implements some functions for multi-component commands,
/// like VISIBLE, and color/character commands.
struct Visual : public IPTCObject {
	/// Width of screen, in pixels.
	const static int WIDTH = 256;
	/// Height of screen, in pixels.
	const static int HEIGHT = 192;
	
	/// Evaluator used for PTC functions.
	Evaluator& e;
	/// Resources used to generate textures.
	Resources& r;
	
	/// PTC console.
	PTC2Console c;
	/// PTC BG layers.
	Background b;
	/// PTC sprites
	Sprites s;
	/// PTC graphics pages.
	Graphics g;
	/// PTC panel, keyboard and icon management.
	Panel p;
	
	/// Determines which components are visible.
	bool visible[6]; //CON, PNL, FG, BG, SP, GRP
	/// Frames since launching, lower half
	volatile Number* maincntl;
	/// Frames since launching, upper half
	volatile Number* maincnth;
	
	/// Shader to handle rendering textures with palette.
	sf::Shader bgsp_shader;
	/// Texture containing all COL palettes.
	sf::Texture col_tex;
	/// Textures corresponding to each resource type.
	std::vector<sf::Texture> resource_tex;
	/// Texture for rendering GRP pages.
	sf::Texture grp_tex;
	
	//PTC functions
	void visible_(const Args&);
	void vsync_(const Args&);
	
	void chrinit_(const Args&);
	void chrset_(const Args&);
	void chrread_(const Args&);
	void colinit_(const Args&);
	void colset_(const Args&);
	void colread_(const Args&);
	
	void acls_(const Args&);
	void cls_(const Args&);
	void save_(const Args&);
	void load_(const Args&);
	
	/// Regenerates the color texture.
	/// Called after some COL resource is modified, such as by LOAD or COLSET.
	void regen_col();
	/// Regenerates the character texture corresponding to the given type.
	/// Used after something like LOAD or CHRSET. 
	void regen_chr(std::string type);
	
public:
	/// Constructor
	/// 
	/// @param ev Evaluator object
	/// @param rs Resources object
	/// @param i Input object
	Visual(Evaluator& ev, Resources& rs, Input& i);
	
	/// Default constructor (deleted)
	Visual() = delete;
	
	/// Copy constructor (deleted)
	Visual(const Visual&) = delete;
	
	/// Copy assignment (deleted)
	Visual& operator=(const Visual&) = delete;
	
	/// Returns a map of tokens to commands. Includes commands from contained
	/// objects, like the console, panel, sprites, etc.
	/// 
	/// @return Map of Tokens to PTC commands
	std::map<Token, cmd_type> get_cmds() override;
	/// Returns a map of tokens to functions. Includes functions from contained
	/// objects, like the console, panel, sprites, etc.
	/// 
	/// @return Map of Tokens to PTC functions
	std::map<Token, op_func> get_funcs() override;
	
	/// Advances by one frame. Updates MAINCNTL, as well as any objects with
	/// animation, like sprites or BG layers.
	void update();
	
	/// Draws every PTC object that requires rendering.
	/// 
	/// @param window Window to render to
	void draw(sf::RenderWindow& window);
	
	/// Sets the render states to match the CHR and COL provided.
	/// Also sets a uniform in bgsp_shader.
	/// 
	/// @param chr Character texture to use
	/// @param col Color palette to use
	/// @return RenderStates for combination
	sf::RenderStates set_state(int chr, float col);
};
