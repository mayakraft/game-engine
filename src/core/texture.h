#pragma once

#include "glad.h"
#ifdef _WIN32
#include <SDL.h>
#include <SDL_image.h>
#else
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

GLuint loadGLTexture(
	unsigned char *data,
	int width,
	int height,
	GLuint inputFormat,
	GLuint storageFormat,
	unsigned short blendMode
);

GLuint updateGLTexture(
	GLuint texture,
	unsigned char *data,
	int width,
	int height,
	GLuint glImageFormat,
	unsigned short blendMode
);

GLuint loadGLTextureFromFileRGB(const char* path);
GLuint loadGLTextureFromFileRGBWithBlend(const char* path, unsigned short blendMode);

GLuint loadGLTextureFromFileRGBA(const char* path);
GLuint loadGLTextureFromFileRGBAWithBlend(const char* path, unsigned short blendMode);

GLuint loadGLTextureFromFileGrayscale(const char* path);
GLuint loadGLTextureFromFileGrayscaleWithBlend(const char* path, unsigned short blendMode);

SDL_Texture* loadSDLTexture(SDL_Renderer *renderer, const char* path);

#ifdef __cplusplus
}
#endif
