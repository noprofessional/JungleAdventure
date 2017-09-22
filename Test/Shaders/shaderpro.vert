#version 130

in vec2 vertexPosition;
in vec4 vertexColor;
in vec2 vertexuv;
out vec4 fragmentColor;
varying vec2 fragmentuv;

uniform mat4 P;

void main(){
	gl_Position.xy= (P*vec4(vertexPosition.xy,0,1.0)).xy;
	gl_Position.z = 0.0;
	gl_Position.w = 1.0;

	fragmentColor = vertexColor;
	fragmentuv = vec2(vertexuv.x,1.0-vertexuv.y);
}
