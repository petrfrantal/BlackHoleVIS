#version 150

in vec3 position;
in vec3 normal;

uniform mat4 MVP;

smooth out vec3 position_v;		
smooth out vec3 normal_v;

void main()
{
	// we let the OpenGL pipeline to interpolate positions and normals
	position_v = position;
	normal_v = normal;
	gl_Position = MVP * vec4(position, 1.0);
}
