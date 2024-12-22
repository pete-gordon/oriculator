/*
 **  Oricutron
 **  Copyright (C) 2009-2014 Peter Gordon
 **
 **  This program is free software; you can redistribute it and/or
 **  modify it under the terms of the GNU General Public License
 **  as published by the Free Software Foundation, version 2
 **  of the License.
 **
 **  This program is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with this program; if not, write to the Free Software
 **  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 **
 **  SDL System specific stuff
 */


#define WANT_WMINFO

// NOTE: SDL/SDL2 for macos have SDL_SysWMinfo
// NOTE: check this code before removing
// #ifndef __APPLE__
// #define WANT_WMINFO
// #else
// #define SDL_SysWMinfo void
// #endif

#include "system.h"

#if SDL_MAJOR_VERSION == 1
#ifdef __SPECIFY_SDL_DIR__
#include <SDL/SDL_endian.h>
#else
#include <SDL_endian.h>
#endif
#else
#ifdef __SPECIFY_SDL_DIR__
#include <SDL2/SDL_endian.h>
#else
#include <SDL_endian.h>
#endif
#endif

#ifdef __OPENGL_AVAILABLE__
#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#endif
#endif

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
# define RMASK 0xFF000000
# define GMASK 0x00FF0000
# define BMASK 0x0000FF00
# define AMASK 0x000000FF
#else
# define RMASK 0x000000FF
# define GMASK 0x0000FF00
# define BMASK 0x00FF0000
# define AMASK 0xFF000000
#endif

#if SDL_MAJOR_VERSION == 1
static SDL_Surface* g_icon = NULL;

static void FreeResources(void)
{
  // nope
}
#else
static SDL_bool g_fullscreen = SDL_FALSE;
static SDL_GLContext g_glcontext = NULL;
static SDL_Renderer* g_renderer = NULL;
static SDL_Texture *g_texture = NULL;
static SDL_Surface* g_screen = NULL;
static SDL_Window* g_window = NULL;
static SDL_Surface* g_icon = NULL;

static int g_bpp = 0;
static int g_width = 0;
static int g_height = 0;

static int g_lastx = SDL_WINDOWPOS_CENTERED;
static int g_lasty = SDL_WINDOWPOS_CENTERED;

static void FreeResources(void)
{
  if(g_glcontext)
  {
    SDL_GL_DeleteContext(g_glcontext);
    g_glcontext = NULL;
  }

  if (g_texture)
  {
    SDL_DestroyTexture(g_texture);
    g_texture = NULL;
  }

  if(g_renderer)
  {
    SDL_DestroyRenderer(g_renderer);
    g_renderer = NULL;
  }

  if(g_screen)
  {
    SDL_FreeSurface(g_screen);
    g_screen = NULL;
  }

  if(g_window)
  {
    SDL_GetWindowPosition(g_window, &g_lastx, &g_lasty);
    SDL_DestroyWindow(g_window);
    g_window = NULL;
  }

}
#endif

int SDL_COMPAT_Init(Uint32 flags)
{
  int rc;

#ifdef DEBUG_SDLINFO
  SDL_version ver;
  SDL_VERSION(&ver);
  fprintf(stderr,"SDL version %u.%u.%u", ver.major, ver.minor, ver.patch);

#if SDL_MAJOR_VERSION == 1
  // SDL_GetVersion not defined in SDL-1.x
#else
  SDL_GetVersion(&ver);
  fprintf(stderr,"/%u.%u.%u", ver.major, ver.minor, ver.patch);
#endif

  fprintf(stderr,"\n");
#endif

  rc = SDL_Init( flags );
  if( rc < 0 )
    error_printf( "SDL init failed: %s", SDL_GetError() );

  return rc;
}

#if SDL_MAJOR_VERSION == 1
int SDL_COMPAT_GetWMInfo(SDL_SysWMinfo *info)
{
  // NOTE: see above for SDL_GetWMInfo in macros
  // #if defined(__MORPHOS__)||defined(__APPLE__)
  #if defined(__MORPHOS__)
  return 0;
  #else
  return SDL_GetWMInfo(info);
  #endif
}
#else
int SDL_COMPAT_GetWMInfo(SDL_SysWMinfo *info)
{
  return g_window? SDL_GetWindowWMInfo(g_window, info) : -1;
}
#endif

// NOTE: same for both SDL versions
void SDL_COMPAT_WM_SetIcon(SDL_Surface *icon, Uint8 *mask)
{
  if(g_icon)
  {
    SDL_FreeSurface(g_icon);
    g_icon = NULL;
  }
  if(icon)
    g_icon = icon;
}

#if SDL_MAJOR_VERSION == 1
void SDL_COMPAT_WM_SetCaption(const char *title, const char *icon)
{
  SDL_WM_SetCaption(title, icon);
}
#else
void SDL_COMPAT_WM_SetCaption(const char *title, const char *icon)
{
  if(g_window)
    SDL_SetWindowTitle(g_window, title);
}
#endif


#if SDL_MAJOR_VERSION == 1
SDL_bool SDL_COMPAT_IsAppActive(SDL_Event* event)
{
  SDL_ActiveEvent *act_ev = (SDL_ActiveEvent*)event;
  return (act_ev->state == SDL_APPACTIVE && act_ev->gain == 1)? SDL_TRUE : SDL_FALSE;
}
SDL_bool SDL_COMPAT_IsAppFocused(SDL_Event* event)
{
  switch( event->active.type )
  {
    case SDL_APPINPUTFOCUS:
    case SDL_APPACTIVE:
      return SDL_TRUE;
      break;
  }
  return SDL_FALSE;
}
#else
SDL_bool SDL_COMPAT_IsAppActive(SDL_Event* event)
{
  /* NOTE: not needed with SDL 2.0
   *  return (event->window.event == SDL_WINDOWEVENT_FOCUS_GAINED)? SDL_TRUE : SDL_FALSE; */
  return SDL_FALSE;
}
SDL_bool SDL_COMPAT_IsAppFocused(SDL_Event* event)
{
  /* NOTE: not needed with SDL 2.0
   *  switch( event->window.event  )
   *  {
   *    case SDL_WINDOWEVENT_FOCUS_GAINED:
   *    case SDL_WINDOWEVENT_ENTER:
   *      return SDL_TRUE;
   *      break;
} */
  return SDL_FALSE;
}
#endif

#if SDL_MAJOR_VERSION == 1
int SDL_COMPAT_EnableKeyRepeat(int delay, int interval)
{
  return SDL_EnableKeyRepeat(delay, interval);
}
#else
int SDL_COMPAT_EnableKeyRepeat(int delay, int interval)
{
  /* This is gone in SDL2 */
  return 0;
}
#endif


#if SDL_MAJOR_VERSION == 1
int SDL_COMPAT_EnableUNICODE(int enable)
{
  return SDL_EnableUNICODE(enable);
}
#else
int SDL_COMPAT_EnableUNICODE(int enable)
{
  /* It's always enabled */
  return 0;
}
#endif

// NOTE: uncomment to have key codes in monitor mode
// dumped to con and to 'key_dump.txt' file
// #define DEBUG_KEY_DUMP
#ifdef DEBUG_KEY_DUMP
static FILE* fkd = 0;
static const char* skd = "key_dump.txt";
#define key_dump(x...) \
{ printf(x); if(!fkd) fkd=fopen(skd, "a"); \
  if(fkd) fprintf(fkd,x); }
#endif

#if SDL_MAJOR_VERSION == 1
SDL_COMPAT_KEY SDL_COMPAT_GetKeysymUnicode(SDL_COMPAT_KEYSYM keysym)
{
  // FIXME
#ifdef DEBUG_KEY_DUMP
  char c = 0x7f & keysym.sym; c = (c < ' ')? ' ' : c;
  key_dump("SDL : GetKeysymUnicode: "
  "c=|%c| scancode=$%.4X, sym=$%.4X, mod=$%.4X, unicode=$%.4X -> unicode=$%.4X\n",
  c, keysym.scancode, keysym.sym, keysym.mod, keysym.unicode, keysym.unicode);
#endif
  // FIXME

  return keysym.unicode;
}
SDL_COMPAT_KEY SDL_COMPAT_TranslateUnicode(SDL_COMPAT_KEYSYM keysym)
{
  // FIXME
#ifdef DEBUG_KEY_DUMP
  char c = 0x7f & keysym.sym; c = (c < ' ')? ' ' : c;
  key_dump("SDL : TranslateUnicode: "
  "c=|%c| scancode=$%.4X, sym=$%.4X, mod=$%.4X, unicode=$%.4X -> unicode=$%.4X\n",
  c, keysym.scancode, keysym.sym, keysym.mod, keysym.unicode, keysym.unicode);
#endif
  // FIXME
  return keysym.unicode;
}
#else
SDL_COMPAT_KEY SDL_COMPAT_GetKeysymUnicode(SDL_COMPAT_KEYSYM keysym)
{
  // FIXME
#ifdef DEBUG_KEY_DUMP
  char c = 0x7f & keysym.sym; c = (c < ' ')? ' ' : c;
  key_dump("SDL2: GetKeysymUnicode: "
  "c=|%c| scancode=$%.4X, sym=$%.4X, mod=$%.4X, unicode=----- -> keysym.sym=$%.4X\n",
  c, keysym.scancode, keysym.sym, keysym.mod, keysym.sym);
#endif
  // FIXME
  return keysym.sym;
}
SDL_COMPAT_KEY SDL_COMPAT_TranslateUnicode(SDL_COMPAT_KEYSYM keysym)
{
  // FIXME
  SDL_COMPAT_KEY sym_out = keysym.sym;
#ifdef DEBUG_KEY_DUMP
  SDL_COMPAT_KEY sym_in = keysym.sym;
#endif
  // FIXME
  if(keysym.mod & KMOD_SHIFT)
  {
    switch(keysym.sym)
    {
      case '0'  : sym_out = ')'; break;
      case '1'  : sym_out = '!'; break;
      case '2'  : sym_out = '@'; break;
      case '3'  : sym_out = '#'; break;
      case '4'  : sym_out = '$'; break;
      case '5'  : sym_out = '%'; break;
      case '6'  : sym_out = '^'; break;
      case '7'  : sym_out = '&'; break;
      case '8'  : sym_out = '*'; break;
      case '9'  : sym_out = '('; break;
      case ';'  : sym_out = ':'; break;
      case '\'' : sym_out = '"'; break;
      case '\\' : sym_out = '|'; break;
      case ','  : sym_out = '<'; break;
      case '.'  : sym_out = '>'; break;
      case '/'  : sym_out = '?'; break;
      case '`'  : sym_out = '~'; break;
      case '-'  : sym_out = '_'; break;
      case '='  : sym_out = '+'; break;
      case '['  : sym_out = '{'; break;
      case ']'  : sym_out = '}'; break;
      default:
        sym_out = keysym.sym = toupper(keysym.sym);
        break;
    }
  }
  // FIXME
#ifdef DEBUG_KEY_DUMP
  char c = 0x7f & keysym.sym; c = (c < ' ')? ' ' : c;
  key_dump("SDL2: TranslateUnicode: "
  "c=|%c| scancode=$%.4X, sym=$%.4X, mod=$%.4X, unicode=----- -> keysym.sym=$%.4X\n",
  c, keysym.scancode, sym_in, keysym.mod, sym_out);
#endif
  // FIXME

  return sym_out;
}
#endif

#if SDL_MAJOR_VERSION == 1
int SDL_COMPAT_Flip(SDL_Surface* screen)
{
  return SDL_Flip(screen);
}
#else
int SDL_COMPAT_Flip(SDL_Surface* screen)
{
  if(g_renderer)
  {
    SDL_UpdateTexture(g_texture, NULL, g_screen->pixels, g_screen->pitch);
    SDL_RenderClear(g_renderer);
    SDL_RenderCopy(g_renderer, g_texture, NULL, NULL);
    SDL_RenderPresent(g_renderer);
  }
  return 0;
}
#endif

#if SDL_MAJOR_VERSION == 1
int SDL_COMPAT_GetBitsPerPixel(void)
{
  const SDL_VideoInfo* info = SDL_GetVideoInfo();
  return (info)? info->vfmt->BitsPerPixel : 0;
}
#else
int SDL_COMPAT_GetBitsPerPixel(void)
{
  int bpp;
  SDL_DisplayMode mode;
  SDL_GetCurrentDisplayMode(0, &mode);
  bpp = SDL_BITSPERPIXEL(mode.format);
  return (bpp==24)? 32 : bpp;
}
#endif

#if SDL_MAJOR_VERSION == 1
int SDL_COMPAT_WM_ToggleFullScreen(SDL_Surface *surface)
{
  return SDL_WM_ToggleFullScreen(surface);
}
#else
int SDL_COMPAT_WM_ToggleFullScreen(SDL_Surface *surface)
{
  if(g_window)
  {
    g_fullscreen = (g_fullscreen)? SDL_FALSE : SDL_TRUE;

    if(g_fullscreen)
      SDL_SetWindowFullscreen(g_window, SDL_WINDOW_FULLSCREEN);
    else
      SDL_SetWindowSize(g_window, g_width, g_height);
  }

  return 0;
}
#endif

#if SDL_MAJOR_VERSION == 1
SDL_Surface* SDL_COMPAT_SetVideoMode(int width, int height, int bitsperpixel, Uint32 flags)
{
  FreeResources();

  // NOTE: SDL_WM_SetIcon function must be called before the first call to SDL_SetVideoMode.
  if (g_icon)
    SDL_WM_SetIcon(g_icon, NULL);

  return SDL_SetVideoMode(width, height, bitsperpixel, flags);
}
#else
SDL_Surface* SDL_COMPAT_SetVideoMode(int width, int height, int bitsperpixel, Uint32 flags)
{
  g_width = width;
  g_height = height;
  g_bpp = bitsperpixel;

  FreeResources();

  // When leaving fullscreen mode, X and Y coordinates
  // should be recentered relative to the display.
  if ( !(flags & SDL_WINDOW_FULLSCREEN) )
  {
      g_lastx = SDL_WINDOWPOS_CENTERED;
      g_lasty = SDL_WINDOWPOS_CENTERED;
  }

#ifndef __ANDROID__
  g_window = SDL_CreateWindow("oricutron", g_lastx, g_lasty,
                              g_width, g_height, flags);
#else
  g_window = SDL_CreateWindow("oricutron", SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
                              0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
#endif
  if (g_icon)
    SDL_SetWindowIcon(g_window, g_icon);

  if(flags & SDL_WINDOW_OPENGL)
  {
    g_screen = SDL_GetWindowSurface(g_window);
    g_glcontext = SDL_GL_CreateContext(g_window);

    SDL_GL_SetSwapInterval(1);
  }
  else
  {
    g_screen = SDL_CreateRGBSurface(0, g_width, g_height, g_bpp,
                                    RMASK, GMASK, BMASK, AMASK);
    g_renderer = SDL_CreateRenderer(g_window, -1, 0);
#ifdef __ANDROID__
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");//"linear");
    SDL_RenderSetLogicalSize(g_renderer, g_width, g_height);
#endif
    g_texture = SDL_CreateTexture(g_renderer,
                                  SDL_PIXELFORMAT_ABGR8888,
                                  SDL_TEXTUREACCESS_STREAMING,
                                  g_width, g_height);
  }

  return g_screen;
}
#endif

#if SDL_MAJOR_VERSION == 1
int SDL_COMPAT_SetPalette(SDL_Surface *surface, int flags, SDL_Color *colors, int firstcolor, int ncolors)
{
  return SDL_SetPalette(surface, flags, colors, firstcolor, ncolors);
}
#else
int SDL_COMPAT_SetPalette(SDL_Surface *surface, int flags, SDL_Color *colors, int firstcolor, int ncolors)
{
  /* FIXME */
  return 0;
}
#endif

#if SDL_MAJOR_VERSION == 1
void SDL_COMPAT_SetEventFilter(SDL_EventFilter filter)
{
  SDL_SetEventFilter(filter);
}
#else
void SDL_COMPAT_SetEventFilter(SDL_EventFilter filter)
{
  SDL_SetEventFilter(filter, NULL);
}
#endif

// NOTE: same for both SDL versions
void SDL_COMPAT_Quit(SDL_bool freeall)
{
  FreeResources();
  if (g_icon && freeall)
  {
    // ...and the surface containing the icon pixel data is no longer required.
    SDL_FreeSurface(g_icon);
    g_icon = NULL;
  }
  SDL_Quit();
}

#ifdef __OPENGL_AVAILABLE__
#if SDL_MAJOR_VERSION == 1
void SDL_COMPAT_GL_SwapBuffers(void)
{
  SDL_GL_SwapBuffers();
}
#else
void SDL_COMPAT_GL_SwapBuffers(void)
{
  SDL_GL_SwapWindow(g_window);
}
#endif
#endif

#ifdef __OPENGL_AVAILABLE__
SDL_Surface * flipVert(SDL_Surface* sfc)
{
  int line;
  SDL_Surface* result = SDL_CreateRGBSurface(sfc->flags, sfc->w, sfc->h,
                                             sfc->format->BytesPerPixel * 8, sfc->format->Rmask, sfc->format->Gmask,
                                             sfc->format->Bmask, sfc->format->Amask);

  if (result)
  {
    Uint8* pixels = (Uint8*) sfc->pixels;
    Uint8* rpixels = (Uint8*) result->pixels;

    Uint32 pitch = sfc->pitch;
    Uint32 pxlength = pitch*sfc->h;

    for(line = 0; line < sfc->h; ++line)
    {
      Uint32 pos = line * pitch;
      memcpy(&rpixels[pos], &pixels[(pxlength-pos)-pitch], pitch);
    }
  }

  return result;
}
#if SDL_MAJOR_VERSION == 1
void SDL_COMPAT_TakeScreenshot(char *fname)
{
  SDL_Surface *g_screen = SDL_GetVideoSurface();

  if(g_screen->flags & SDL_COMPAT_OPENGL)
  {
    // Juste pour recuperer les dimensions :/
    int g_width = g_screen->w;
    int g_height = g_screen->h;
    SDL_Surface *flip;

    // 24 Bits
    SDL_Surface *surf = SDL_CreateRGBSurface(SDL_SWSURFACE, g_width, g_height, 24, RMASK, GMASK, BMASK, 0);
    glReadPixels(0, 0, g_width, g_height, GL_RGB, GL_UNSIGNED_BYTE, surf->pixels);

    // 32 Bits
    //SDL_Surface *surf = SDL_CreateRGBSurface(SDL_SWSURFACE, g_width, g_height, 32, RMASK, GMASK, BMASK, AMASK);
    //glReadPixels(0, 0, g_width, g_height, GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels);

    flip = flipVert(surf);
    SDL_SaveBMP(flip, fname);

    SDL_FreeSurface(flip);
    SDL_FreeSurface(surf);
  }
  else
    SDL_SaveBMP(g_screen, fname);
}
#else
void SDL_COMPAT_TakeScreenshot(char *fname)
{
  // 24 Bits
  // SDL_Surface *surf = SDL_CreateRGBSurface(SDL_SWSURFACE, g_width, g_height, 24, RMASK, BMASK, GMASK, 0);

  // 32 Bits
  SDL_Surface *surf = SDL_CreateRGBSurface(SDL_SWSURFACE, g_width, g_height, g_bpp, RMASK, GMASK, BMASK, AMASK);

  if (surf == NULL)
    return;

  // 24 Bits
  // glReadPixels(0,0,g_width, g_height, GL_RGB, GL_UNSIGNED_BYTE, surf->pixels);

  // 32 Bits
  glReadPixels(0,0,g_width, g_height, GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels);

  SDL_Surface *flip = flipVert(surf);
  SDL_SaveBMP(flip, fname);

  SDL_FreeSurface(flip);
  SDL_FreeSurface(surf);
}
#endif
#else
#if SDL_MAJOR_VERSION == 1
void SDL_COMPAT_TakeScreenshot(char *fname)
{
  SDL_SaveBMP(SDL_GetVideoSurface(), fname);
}
#else
void SDL_COMPAT_TakeScreenshot(char *fname)
{
  SDL_SaveBMP(g_screen, fname);
}
#endif
#endif

// This is an ugly hack for OSX will be removed
#ifdef __APPLE__
int32_t __isPlatformVersionAtLeast(int32_t Major, int32_t Minor, int32_t Subminor)
{
  (void) Major;
  (void) Minor;
  (void) Subminor;
  return SDL_TRUE;
}
#endif
