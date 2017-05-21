//----------------------------------------------------------------------------------------
/**
* \file       Window.h
* \author     Petr Frantal
* \date       2017/02/10
*
*  Defines the class used for maintaining the SDL2 window and coordinate it with OpenGL.
*
*/
//----------------------------------------------------------------------------------------

#pragma once

#include <SDL.h>
#include <glew.h>
#include <string>
#include <iostream>

/**
*	Class handling the SDL2 window with OpenGL context.
*/
class Window {
	private:
		/// Pointer to an SDL2 window
		SDL_Window * window;

		/// OpenGL context
		SDL_GLContext glContext;
	public:
		/**
		*	Initializes the SDL2, OpenGL and glew.
		*	Creates the window with given name, width and height.
		*/
		Window(std::string & windowName, int width, int height);

		/**
		*	Disposes all the allocated data.
		*/
		~Window(void);

		/**
		*	Calls glClear() with given color specification.
		*/
		void clear(float r, float g, float b, float a);

		/**
		*	Swaps the frame buffers (we use double buffering).
		*/
		void swapBuffers(void);
};
