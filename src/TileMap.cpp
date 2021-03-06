#include "TileMap.hpp"

TileMap::TileMap(int w, int h) : width{w}, height{h}{
	va = sf::VertexArray(sf::Quads, w*h*4);
	
	for (int y = 0; y < h; ++y){
		for (int x = 0; x < w; ++x){
			va[4*(x+w*y)].position = sf::Vector2f(x*8, y*8);
			va[4*(x+w*y)+1].position = sf::Vector2f(x*8+8, y*8);
			va[4*(x+w*y)+2].position = sf::Vector2f(x*8+8, y*8+8);
			va[4*(x+w*y)+3].position = sf::Vector2f(x*8, y*8+8);
			//vertex color = palette info
			va[4*(x+w*y)].color = sf::Color(0);
			va[4*(x+w*y)+1].color = sf::Color(0);
			va[4*(x+w*y)+2].color = sf::Color(0);
			va[4*(x+w*y)+3].color = sf::Color(0);
		}
	}
}

sf::Vector2f chr_texCoords(int chr, int xofs = 0, int yofs = 0){
	int x = chr % 32;
	int y = chr / 32;
	return sf::Vector2f(8*(x+xofs), 8*(y+yofs));
}

//sets a single tile, by means of setting the texture coordinates for the tile
void TileMap::tile(int x, int y, int chr){
	va[4*(x+width*y)].texCoords = chr_texCoords(chr);
	va[4*(x+width*y)+1].texCoords = chr_texCoords(chr, 1);
	va[4*(x+width*y)+2].texCoords = chr_texCoords(chr, 1, 1);
	va[4*(x+width*y)+3].texCoords = chr_texCoords(chr, 0, 1);
}

void TileMap::tile(int x, int y, int chr, bool h, bool v){
	va[4*(x+width*y)].texCoords = chr_texCoords(chr, h ? 1 : 0, v ? 1 : 0);
	va[4*(x+width*y)+1].texCoords = chr_texCoords(chr, h ? 0 : 1, v ? 1 : 0);
	va[4*(x+width*y)+2].texCoords = chr_texCoords(chr, h ? 0 : 1, v ? 0 : 1);
	va[4*(x+width*y)+3].texCoords = chr_texCoords(chr, h ? 1 : 0, v ? 0 : 1);
}

//sets the colors of a tile (background only applies when tex color is 0)
void TileMap::palette(int x, int y, int pal, int bg){
	va[4*(x+width*y)].color = sf::Color(pal,bg,0);
	va[4*(x+width*y)+1].color = sf::Color(pal,bg,0);
	va[4*(x+width*y)+2].color = sf::Color(pal,bg,0);
	va[4*(x+width*y)+3].color = sf::Color(pal,bg,0);
}

void TileMap::draw(sf::RenderTarget& t, sf::RenderStates s) const {
	s.transform *= getTransform();
	t.draw(va, s);
}
