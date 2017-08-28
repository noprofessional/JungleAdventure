#version 130

in vec3 vertexPosition;
in vec4 vertexColor;
in vec2 vertexuv;
out vec4 fragmentColor;
out vec2 fragmentuv;

uniform mat4 P;

void main(){
	gl_Position.xy= (P*vec4(vertexPosition.xy,0,vertexPosition.z)).xy;
	gl_Position.z = 0.0;
	gl_Position.w = vertexPosition.z;

	fragmentColor = vertexColor;
	fragmentuv = vertexuv;
}
