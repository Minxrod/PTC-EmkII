#include "SpriteArray.h"
#include <cmath>

SpriteArray::SpriteArray() {}

//reads from a CHR resource
sf::Vector2f chr_texCoords2(int chr, int xofs = 0, int yofs = 0){
	int x = chr % 32;
	int y = chr / 32;
	return sf::Vector2f(8*(x+xofs), 8*(y+yofs));
}

void rotate_xy(double& x, double& y, const double& a){
	double ox = x;
	x = x * std::cos(a) + y * std::sin(a);
	y = ox*-std::sin(a) + y * std::cos(a);
}

void translate_xy(double& x, double& y, const double& dx, const double& dy){
	x += dx;
	y += dy;
}

/*bool is_hit(SpriteInfo& a, SpriteInfo& b){
	if (!a.active || !b.active)
		return false; //can't hit inactive sprites..
	
	double w = a.w * a.scale.s;
	double h = a.h * a.scale.s;

	double ax1 = -a.scale.s * a.home_x;
	double ay1 = -a.scale.s * a.home_y;
	double ax2 = -a.scale.s * a.home_x + w;
	double ay2 = -a.scale.s * a.home_y;
	double ax3 = -a.scale.s * a.home_x + w;
	double ay3 = -a.scale.s * a.home_y + h;
	double ax4 = -a.scale.s * a.home_x;
	double ay4 = -a.scale.s * a.home_y + h;
	
	
}*/

void SpriteArray::add_sprite(const SpriteInfo& s){	
	//Vertex order:
	//1--2
	//|  |
	//|  |
	//4--3
	double w = s.w * s.scale.s;
	double h = s.h * s.scale.s;

	//determine scaled sprite box
	double x1 = -s.scale.s * s.home_x;
	double y1 = -s.scale.s * s.home_y;
	double x2 = -s.scale.s * s.home_x + w;
	double y2 = -s.scale.s * s.home_y;
	double x3 = -s.scale.s * s.home_x + w;
	double y3 = -s.scale.s * s.home_y + h;
	double x4 = -s.scale.s * s.home_x;
	double y4 = -s.scale.s * s.home_y + h;

	if (s.flip_x){
		std::swap(x1,x2);
		std::swap(y1,y2);
		std::swap(x4,x3);
		std::swap(y4,y3);
	}
	if (s.flip_y){
		std::swap(x1,x4);
		std::swap(y1,y4);
		std::swap(x2,x3);
		std::swap(y2,y3);
	}
	//rotate around s.home_x, s.home_y
	double a = s.angle.a / 180.0 * 3.141592653589;
	rotate_xy(x1,y1,a);
	rotate_xy(x2,y2,a);
	rotate_xy(x3,y3,a);
	rotate_xy(x4,y4,a);

	//move rotated+scaled to correct location
	double x = s.pos.x - s.scale.s * s.home_x;
	double y = s.pos.y - s.scale.s * s.home_y;
	translate_xy(x1,y1,x,y);
	translate_xy(x2,y2,x,y);
	translate_xy(x3,y3,x,y);
	translate_xy(x4,y4,x,y);

	//1--2
	//|
	//|
	//4
	double xcstep_tx = (x2 - x1) / s.w * 8;
	double ycstep_tx = (y2 - y1) / s.w * 8;
	double xcstep_ty = (x4 - x1) / s.h * 8;
	double ycstep_ty = (y4 - y1) / s.h * 8;
	sf::Color c{(unsigned char)(16*s.pal), 0, 0};

	double xc = x1;
	double yc = y1;
	int chr = 4*s.chr;
	for (int ty = 0; ty < s.h/8 ; ++ty){
		for (int tx = 0; tx < s.w/8; ++tx){
			va.append(sf::Vertex(sf::Vector2f(xc, yc), c, chr_texCoords2(chr,0,0)));
			va.append(sf::Vertex(sf::Vector2f(xc+xcstep_tx, yc+ycstep_tx), c, chr_texCoords2(chr,1,0)));
			va.append(sf::Vertex(sf::Vector2f(xc+xcstep_tx+xcstep_ty, yc+ycstep_tx+ycstep_ty), c, chr_texCoords2(chr,1,1)));
			va.append(sf::Vertex(sf::Vector2f(xc+xcstep_ty, yc+ycstep_ty), c, chr_texCoords2(chr,0,1)));
			
			translate_xy(xc, yc, xcstep_tx, ycstep_tx);
			++chr;
		}
		translate_xy(xc, yc, -s.w/8*xcstep_tx, -s.w/8*ycstep_tx);
		translate_xy(xc, yc, xcstep_ty, ycstep_ty);
	}
}

void SpriteArray::draw(sf::RenderTarget& r, sf::RenderStates s) const{
	s.transform *= getTransform();
	r.draw(va, s);
}
