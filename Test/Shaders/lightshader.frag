#version 130

in vec4 fragmentColor;
in vec2 fragmentuv;
out vec4 color;

void main(){
	float distance = length(fragmentuv);
	color = vec4(fragmentColor.rgb,fragmentColor.a*(pow(0.01,distance)-0.01));
}
