/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2017 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "../../SDL_internal.h"

#if SDL_VIDEO_DRIVER_VITA_GL

#include <stdlib.h>
#include <string.h>

#include "SDL_error.h"
#include "SDL_vitavideo.h"
#include "SDL_vitagl.h"

/*****************************************************************************/
/* SDL OpenGL functions                                            			*/
/*****************************************************************************/
#define GLCHK(stmt)                             \
    do {                                        \
        GLenum err;                             \
                                                \
        stmt;                                   \
        err = glGetError();                     \
        if (err != GL_NO_ERROR) {               \
            SDL_SetError("GL error %d", err);   \
            return 0;                           \
        }                                       \
    } while (0)

int
VITA_GL_LoadLibrary(_THIS, const char *path)
{
	vita2d_init();
	return 0;
}

void
VITA_GL_UnloadLibrary(_THIS)
{
	vita2d_wait_rendering_done();
	vita2d_fini();
}

void *
VITA_GL_GetProcAddress(_THIS, const char *proc)
{
	return OSMesaGetProcAddress(proc);
}

SDL_GLContext
VITA_GL_CreateContext(_THIS, SDL_Window * window)
{
	SDL_WindowData *wdata = (SDL_WindowData *) window->driverdata;

	//GLint cBits;
	const GLint z = _this->gl_config.depth_size;
	const GLint stencil = _this->gl_config.stencil_size;
	const GLint accum = 0;
	
	_this->gl_config.red_size = 8;
	_this->gl_config.green_size = 8;
	_this->gl_config.blue_size = 8;
	_this->gl_config.alpha_size = 8;
	
	_this->gl_data->context = OSMesaCreateContextExt(OSMESA_RGBA, z, stencil, accum, NULL);
    if (!_this->gl_data->context) {
		SDL_SetError("OSMesaCreateContextExt() failed!\n");
		return 0;
	}

	_this->gl_data->texture = vita2d_create_empty_texture_format(window->w, window->h, SCE_GXM_TEXTURE_BASE_FORMAT_U8U8U8U8);
    _this->gl_data->buffer = vita2d_texture_get_datap(_this->gl_data->texture);
	
	if (!OSMesaMakeCurrent(_this->gl_data->context, _this->gl_data->buffer, GL_UNSIGNED_BYTE, window->w, window->h)) 
	{
		SDL_SetError("Unable to make GL context current (OSMesaMakeCurrent)");
        return 0;
    }
	
	wdata->uses_gl = SDL_TRUE;
	window->flags |= SDL_WINDOW_FULLSCREEN;
	
	return _this->gl_data->context;
}

int
VITA_GL_MakeCurrent(_THIS, SDL_Window * window, SDL_GLContext context)
{
	if (!OSMesaMakeCurrent(_this->gl_data->context, _this->gl_data->buffer, GL_UNSIGNED_BYTE, window->w, window->h)) 
	{
        return SDL_SetError("Unable to make GL context current (OSMesaMakeCurrent)");
    }
    
    return 0;
}

int
VITA_GL_SetSwapInterval(_THIS, int interval)
{
	_this->gl_data->swapinterval = interval;
	vita2d_set_vblank_wait(interval);
    return 0;
}

int
VITA_GL_GetSwapInterval(_THIS)
{
	return _this->gl_data->swapinterval;
}

int
VITA_GL_SwapWindow(_THIS, SDL_Window * window)
{
	
	glFinish();

    vita2d_start_drawing();
    vita2d_clear_screen();
    vita2d_draw_texture_scale_rotate(_this->gl_data->texture, window->w / 2, window->h / 2, -1, 1, 180 * 0.0174532925f);
    vita2d_end_drawing();
    vita2d_wait_rendering_done();
    vita2d_swap_buffers();
    
	return 0;
}

void
VITA_GL_DeleteContext(_THIS, SDL_GLContext context)
{
    if (_this->gl_data->texture) {
        vita2d_free_texture(_this->gl_data->texture);
    }
    
    OSMesaDestroyContext(_this->gl_data->context);
}

#endif /* SDL_VIDEO_DRIVER_VITA_GL */

/* vi: set ts=4 sw=4 expandtab: */
