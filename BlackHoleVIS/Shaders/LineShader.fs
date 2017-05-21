//----------------------------------------------------------------------------------------
/**
* \file       LineShader.fs
* \author     Petr Frantal
* \date       2017/05/20
*
*  The fragment shader definition.
*
*/
//----------------------------------------------------------------------------------------

#version 140

/// Color of the object
in vec3 color_v;

/// Output color value
out vec4 color_f;

void main() {
	color_f = vec4(color_v, 0.0);
}
