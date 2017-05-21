//----------------------------------------------------------------------------------------
/**
* \file       LineShader.vs
* \author     Petr Frantal
* \date       2017/05/20
*
*  The vertex shader definition.
*
*/
//----------------------------------------------------------------------------------------

#version 140

/// input attribute position of the vertex
in vec3 position;
in vec3 color;

/// view projection matrix
uniform mat4 viewProjectionMatrix;

// pass the color to the fragment shader
out vec3 color_v;

void main() {
	gl_Position = viewProjectionMatrix * vec4(position.x, position.y, position.z, 1.0f);
	color_v = color;
}