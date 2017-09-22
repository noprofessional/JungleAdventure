#version 130

in vec4 fragmentColor;
varying vec2 fragmentuv;
out vec4 color;
uniform sampler2D mysampler;

void main(){
	vec4 texturecolor= texture2D(mysampler,fragmentuv);
	color = texturecolor*fragmentColor;
}
