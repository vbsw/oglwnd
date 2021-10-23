#ifndef OGLWND_H
#define OGLWND_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/GL.h>

// from wgl.h
#define WGL_SAMPLE_BUFFERS_ARB            0x2041
#define WGL_SAMPLES_ARB                   0x2042
#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_SWAP_METHOD_ARB               0x2007
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_TYPE_RGBA_ARB                 0x202B
#define WGL_ACCELERATION_ARB              0x2003
#define WGL_FULL_ACCELERATION_ARB         0x2027
#define WGL_SWAP_EXCHANGE_ARB             0x2028
#define WGL_SWAP_COPY_ARB                 0x2029
#define WGL_SWAP_UNDEFINED_ARB            0x202A
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_ALPHA_BITS_ARB                0x201B
#define WGL_DEPTH_BITS_ARB                0x2022
#define WGL_STENCIL_BITS_ARB              0x2023
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001


#define WGL_SWAP_METHOD_EXT               0x2007
#define WGL_SWAP_EXCHANGE_EXT             0x2028
#define WGL_SWAP_COPY_EXT                 0x2029
#define WGL_SWAP_UNDEFINED_EXT            0x202A


#define DUMMY_CLASS_NAME TEXT("vb_cls_dmy")
#define CLASS_NAME TEXT("vb_cls")

/* from wglext.h */
typedef BOOL(WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
typedef HGLRC(WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList);

typedef LRESULT(*cb_t) (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, void *data, BOOL *processed);

static HINSTANCE instance = NULL;
static PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormatARB    = NULL;
static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;

typedef struct {
	WNDCLASSEX cls;
	HWND hndl;
	HDC dc;
	HGLRC rc;
} window_t;

static BOOL     running = FALSE;
static window_t dummy   = { {sizeof(WNDCLASSEX), 0, NULL, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL}, NULL, NULL, NULL };
static window_t window  = { {sizeof(WNDCLASSEX), 0, NULL, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL}, NULL, NULL, NULL };

static struct {
	int x, y, width, height;
	int widthMin, heightMin;
	int widthMax, heightMax;
	int centered;
	int borderless, dragable;
	int resizable, fullscreen;
} config;

/* Go functions can not be passed to c directly.            */
/* They can only be called from c.                          */
/* This code is an indirection to call Go callbacks.        */
/* _cgo_export.h is generated automatically by cgo.         */
#include "_cgo_export.h"

/* Exported functions from Go are:                          */
/* goOnFirstUpdate                                          */
/* goOnUpdate                                               */
/* goOnClose                                                */

#endif /* OGLWND_H */
