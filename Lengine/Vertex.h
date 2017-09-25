#pragma once

#include <GL\glew.h>
namespace Lengine {


//vertex data upload index
	enum Index { POS, COL, UVC };

struct Position {
	float x;
	float y;
	float depth;
};

struct ColorRGBA8 {
	//constructor
	ColorRGBA8():r(0),g(0),b(0),a(0) {}
	ColorRGBA8(GLubyte R, GLubyte G, GLubyte B, GLubyte A) {
		r = R;
		g = G;
		b = B;
		a = A;
	}
	//data
	GLubyte	r;
	GLubyte g;
	GLubyte b;
	GLubyte a;
};

const ColorRGBA8 WHITE(255, 255, 255, 255);
const ColorRGBA8 RED(255, 0, 0, 255);
const ColorRGBA8 BLUE(0, 0, 255, 255);
const ColorRGBA8 GREEN(0, 255, 0, 255);

const ColorRGBA8 MAGENTA(255, 0, 255, 255);
const ColorRGBA8 YELLOW(255, 255, 0, 255);



struct UV {
	float u;
	float v;
};

struct Vertex
{
	Position position;
	ColorRGBA8 color;
	UV uv;

	void setposition(float x, float y,float depth){
		position.x = x;
		position.y = y;
		position.depth = depth;
	}

	void setcolor(GLubyte r, GLubyte g, GLubyte b, GLubyte a){
		color.r = r;
		color.g = g;
		color.b = b;
		color.a = a;
	}

	void setUV(float u, float v){
		uv.u = u;
		uv.v = v;
	}
};

}
