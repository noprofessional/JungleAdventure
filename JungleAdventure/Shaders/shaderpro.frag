#version 130

in vec4 fragmentColor;
in vec2 fragmentuv;
out vec4 color;
uniform sampler2D mysampler;

void main(){
	vec4 texturecolor= texture(mysampler,fragmentuv);
	color = texturecolor*fragmentColor;
}
