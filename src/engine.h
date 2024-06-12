#pragma once

#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif

#include <GL/glew.h>
#ifdef _WIN32
#include <SDL.h>
#include <SDL_image.h>
#else
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#endif

typedef struct InitParams {
	unsigned int flags;
	const char* title;
	int width;
	int height;
	unsigned char disableShaders;
} InitParams;

typedef struct GameEngine {
	SDL_Window* window;
	SDL_Renderer* renderer; // only by calling init2D
	float pixelScale;
	int width;
	int height;
} GameEngine;

float setRendererPixelScale(SDL_Window* window, SDL_Renderer *renderer) {
	int winW, winH, renW, renH;
	SDL_GetWindowSize(window, &winW, &winH);
	SDL_GetRendererOutputSize(renderer, &renW, &renH);
	if (renW == winW) { return 1.0; }
	float widthScale = (float)renW / (float) winW;
	float heightScale = (float)renH / (float) winH;
	// SDL_RenderSetScale(renderer, widthScale, heightScale);
	// SDL_RenderSetLogicalSize(renderer, renW, renH);
	// SDL_Rect topLeftViewport;
	// topLeftViewport.x = 0;
	// topLeftViewport.y = 0;
	// topLeftViewport.w = SCREEN_WIDTH / 2;
	// topLeftViewport.h = SCREEN_HEIGHT / 2;
	// SDL_RenderSetViewport(renderer, &topLeftViewport );
	printf("winsize (%d, %d) : renderer (%d, %d) : %f, %f\n",
		winW, winH,
		renW, renH,
		widthScale, heightScale
	);
	return widthScale;
}

void setFullscreenGL(GameEngine *engine, float *aspect) {
	SDL_SetWindowFullscreen(engine->window, SDL_WINDOW_FULLSCREEN);
	// SDL_SetWindowFullscreen(engine->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	int resizeW, resizeH;
	SDL_GetWindowSize(engine->window, &resizeW, &resizeH);
	glViewport(0, 0, resizeW * engine->pixelScale, resizeH * engine->pixelScale);
	if (aspect != NULL) { *aspect = (float)resizeW / resizeH; }
}

// void setFullScreen(GameEngine *engine) {
// 	SDL_SetWindowFullscreen(engine->window, SDL_WINDOW_FULLSCREEN);
// }

// Initialize an SDL window and renderer for a fixed-function
// pipeline OpenGL renderer.
GameEngine init2D(InitParams params) {
	if (SDL_Init(params.flags) < 0) { fputs(SDL_GetError(), stderr); }

	SDL_Window *window = SDL_CreateWindow(params.title,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		params.width,
		params.height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
	// SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN
	if (window == NULL) {fputs(SDL_GetError(), stderr); }

	SDL_Renderer *renderer = SDL_CreateRenderer(
		window,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) { fputs(SDL_GetError(), stderr); }

	float pixelScale = setRendererPixelScale(window, renderer);

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		fputs(IMG_GetError(), stderr);
	}

	GameEngine engine;
	engine.window = window;
	engine.renderer = renderer;
	engine.pixelScale = pixelScale;
	engine.width = (int)(params.width * pixelScale);
	engine.height = (int)(params.height * pixelScale);
	return engine;
}

// Initialize GLEW and an SDL window for a programmable pipeline
// using OpenGL making accessible shaders.
GameEngine init3D(InitParams params) {
	// SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
	if (SDL_Init(params.flags) < 0) {
		fputs(SDL_GetError(), stderr);
	}

	if (params.disableShaders) {
		// should we specify a version?
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	} else {
		// need at least OpenGL 3.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	}
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_Window *window = SDL_CreateWindow(params.title,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		params.width,
		params.height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
	if (window == NULL) { fputs(SDL_GetError(), stderr); }

	// On MacOS, this needs to be called immediately after SDL_CreateWindow,
	// if not, any subsequent SDL method call involving window will somehow remove
	// the "SDL_WINDOW_OPENGL" flat and replace it with Metal, or something.
	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (context == NULL) { fputs(SDL_GetError(), stderr); }

	int getW, getH;
	int getGLW, getGLH;
	SDL_GetWindowSize(window, &getW, &getH);
	SDL_GL_GetDrawableSize(window, &getGLW, &getGLH);
	// printf("SDL_GetWindowSize %d, %d\n", getW, getH);
	// printf("SDL_GL_GetDrawableSize %d, %d\n", getGLW, getGLH);
	float pixelScale = (float)getGLW / (float)getW;

	// initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		const char* errorChar = (char const*)glewGetErrorString(glewError);
		fputs(errorChar, stderr);
	}

	// use vsync
	if (SDL_GL_SetSwapInterval(1) < 0) {
		fputs(SDL_GetError(), stderr);
	}

	// initialize SDL_image
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		fputs(IMG_GetError(), stderr);
	}

	GameEngine engine;
	engine.window = window;
	engine.pixelScale = pixelScale;
	engine.width = (int)(params.width * pixelScale);
	engine.height = (int)(params.height * pixelScale);
	return engine;
}

void dealloc(GameEngine* engine) {
	if (engine->renderer != NULL) SDL_DestroyRenderer(engine->renderer);
	if (engine->window != NULL) SDL_DestroyWindow(engine->window);
	engine->renderer = NULL;
	engine->window = NULL;
	IMG_Quit();
	SDL_Quit();
	// glDeleteProgram(gProgramID);
}

//void drawLoop(GameEngine* engine) {
//    // event will be filled with some kind of input event (if exists)
//    // https://wiki.libsdl.org/SDL2/SDL_Event
//    SDL_Event e;
//    bool quit = false;
//    while (!quit) {
//        while (SDL_PollEvent(&e)) {
//            if (e.type == SDL_QUIT) {
//                quit = true;
//            }
//        }
//        // update, draw
//        SDL_UpdateWindowSurface(engine->window);
//    }
//}

void viewportTest(GameEngine *engine, SDL_Texture *texture) {
	float SCREEN_WIDTH = 640;
	float SCREEN_HEIGHT = 480;
	//Top left corner viewport
	SDL_Rect topLeftViewport;
	topLeftViewport.x = 0;
	topLeftViewport.y = 0;
	topLeftViewport.w = (int)SCREEN_WIDTH / 2;
	topLeftViewport.h = (int)SCREEN_HEIGHT / 2;
	SDL_RenderSetViewport( engine->renderer, &topLeftViewport );
	//Render texture to screen
	SDL_RenderCopy( engine->renderer, texture, NULL, NULL );
	//Bottom viewport
	SDL_Rect bottomViewport;
	bottomViewport.x = 0;
	bottomViewport.y = (int)SCREEN_HEIGHT / 2;
	bottomViewport.w = SCREEN_WIDTH;
	bottomViewport.h = (int)SCREEN_HEIGHT / 2;
	SDL_RenderSetViewport( engine->renderer, &bottomViewport );
	//Render texture to screen
	SDL_RenderCopy( engine->renderer, texture, NULL, NULL );
	//Update screen
	SDL_RenderPresent( engine->renderer );
}
