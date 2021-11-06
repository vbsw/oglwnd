#ifndef OGLWND_H
#define OGLWND_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wchar.h>
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

/* from wglext.h */
typedef BOOL(WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
typedef HGLRC(WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList);

static HINSTANCE                         instance                   = NULL;
static PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormatARB    = NULL;
static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;

typedef struct {
	WNDCLASSEX cls;
	HWND hndl;
	HDC dc;
	HGLRC rc;
} window_t;

typedef struct {
	window_t window;
	void *custom;
} dummy_data_t;

typedef struct {
	window_t window;
	void *go_data;
	void *custom;
} window_data_t;

typedef LRESULT(*cb_t) (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, void *data, BOOL *processed);
typedef void(*new_window_f) (window_data_t **data, HINSTANCE instance, void *go_data, int *err_num, char **err_str_extra);
typedef void(*init_class_f) (window_data_t *data, HINSTANCE instance, int *err_num, char **err_str_extra);
typedef void(*init_window_f) (window_data_t *data, int *err_num, char **err_str_extra);
typedef void(*init_context_f) (window_data_t *data, int *err_num, char **err_str_extra);

void new_window_impl(window_data_t **data, HINSTANCE instance, void *go_data, int *err_num, char **err_str_extra);
void init_class_impl(window_data_t *data, HINSTANCE instance, int *err_num, char **err_str_extra);
void init_window_impl(window_data_t *data, int *err_num, char **err_str_extra);
void init_context_impl(window_data_t *data, int *err_num, char **err_str_extra);

static int is_class_registered(HINSTANCE const instance, LPCTSTR const name) {
	WNDCLASSEXW wcx;
	if (GetClassInfoEx(instance, name, &wcx))
		return 1;
	return 0;
}



typedef struct {
	int x, y;
} t2i_t;

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
	int locked;
	int wnd_x, wnd_y, wnd_width, wnd_height;
} config;

static struct {
	int dragging, dragging_cust, locked;
	int minimized, maximized, resizing;
	int focus;
} state = { 0, 0, 0, 0, 0, 0, 0 };

static struct {
	int x, y, width, height;
	int xW, yW, widthW, heightW;
} client = { 0, 0, 0, 0 };

static struct {
	int x, y, cursor_type;
	HCURSOR cursor;
} mouse = { 0, 0, 0, NULL };

static struct {
	HCURSOR arrow, nwse, ns, nesw, we, cust;
} cursor = { NULL, NULL, NULL, NULL, NULL, NULL };

static struct {
	HMONITOR hndl;
	int x, y, width, height;
} monitor = { NULL, 0, 0, 640, 480 };

static int pressed[255];

/* Go functions can not be passed to c directly.            */
/* They can only be called from c.                          */
/* This code is an indirection to call Go callbacks.        */
/* _cgo_export.h is generated automatically by cgo.         */
#include "_cgo_export.h"

/* Exported functions from Go are:                          */
/* goOnClose                                                */
/* goOnFirstUpdate                                          */
/* goOnUpdate                                               */
/* goOnKeyDown                                              */
/* goOnKeyUp                                                */
/* goOnMove                                                 */
/* goOnResize                                               */
/* goOnFirstWindowSize                                      */
/* goOnMenuEnter                                            */
/* goOnMenuLeave                                            */
/* goOnMaximize                                             */
/* goOnMinimize                                             */
/* goOnRestore                                              */
/* goOnFocusLoose                                           */
/* goOnFocusGain                                            */
/* goOnMouseMove                                            */
/* goOnDragBegin                                            */
/* goOnDragEnd                                              */
/* goOnDragCustBegin                                        */
/* goOnDragCustEnd                                          */
/* goOnResizeBegin                                          */
/* goOnResizeEnd                                            */
/* goOnButtonDown                                           */
/* goOnButtonUp                                             */
/* goOnWheel                                                */


static int get_keycode(const UINT message, const WPARAM wParam, const LPARAM lParam) {
	const int key = (int)(HIWORD(lParam) & 0xff);
	switch (key)
	{
	case 0:  return 0;
	case 1:  return 41;        // ESC         0x29
	case 2:  return 30;        // 1           0x1E
	case 3:  return 31;        // 2           0x1F
	case 4:  return 32;        // 3           0x20
	case 5:  return 33;        // 4           0x21
	case 6:  return 34;        // 5           0x22
	case 7:  return 35;        // 6           0x23
	case 8:  return 36;        // 7           0x24
	case 9:  return 37;        // 8           0x25
	case 10: return 38;        // 9           0x26
	case 11: return 39;        // 0           0x27
	case 12: return 45;        // -           0x2D
	case 13: return 46;        // =           0x2E
	case 14: return 42;        // DELETE      0x2A
	case 15: return 43;        // TAB         0x2B
	case 16: return 20;        // Q           0x14
	case 17: return 26;        // W           0x1A
	case 18: return 8;         // E           0x08
	case 19: return 21;        // R           0x15
	case 20: return 23;        // T           0x17
	case 21: return 28;        // Y           0x1C
	case 22: return 24;        // U           0x18
	case 23: return 12;        // I           0x0C
	case 24: return 18;        // O           0x12
	case 25: return 19;        // P           0x13
	case 26: return 47;        // [           0x2F
	case 27: return 48;        // ]           0x30
	case 28:
		if (HIWORD(lParam) >> 8 & 0x1)
			return 88;         // pad ENTER   0x58
		return 40;             // board ENTER 0x28
	case 29:
		if (wParam == VK_CONTROL) {
			if (HIWORD(lParam) >> 8 & 0x1)
				return 228;    // RCTRL       0xE4
			return 224;        // LCTRL       0xE0
		}
		return 0;
	case 30: return 4;         // A           0x04
	case 31: return 22;        // S           0x16
	case 32: return 7;         // D           0x07
	case 33: return 9;         // F           0x09
	case 34: return 10;        // G           0x0A
	case 35: return 11;        // H           0x0B
	case 36: return 13;        // J           0x0D
	case 37: return 14;        // K           0x0E
	case 38: return 15;        // L           0x0F
	case 39: return 51;        // ;           0x33
	case 40: return 52;        // '           0x34
	case 41: return 53;        // ^           0x35
	case 42: return 225;       // LSHIFT      0xE1
	case 43: return 50;        // ~           0x32
	case 44: return 29;        // Z           0x1D
	case 45: return 27;        // X           0x1B
	case 46: return 6;         // C           0x06
	case 47: return 25;        // V           0x19
	case 48: return 5;         // B           0x05
	case 49: return 17;        // N           0x11
	case 50: return 16;        // M           0x10
	case 51: return 54;        // ,           0x36
	case 52: return 55;        // .           0x37
	case 53:
		if (wParam == VK_DIVIDE)
			return 84;         // pad /       0x54
		return 56;             // /           0x38
	case 54: return 229;       // RSHIFT      0xE5
	case 55: return 85;        // pad *       0x55
	case 56:
		if (message == WM_SYSKEYDOWN || message == WM_SYSKEYUP)
			return 226;        // LALT        0xE2
		return 230;            // RALT        0xE6
	case 57: return 44;        // SPACE       0x2C
	case 58: return 57;        // CAPS        0x39
	case 59: return 58;        // F1          0x3A
	case 60: return 59;        // F2          0x3B
	case 61: return 60;        // F3          0x3C
	case 62: return 61;        // F4          0x3D
	case 63: return 62;        // F5          0x3E
	case 64: return 63;        // F6          0x3F
	case 65: return 64;        // F7          0x40
	case 66: return 65;        // F8          0x41
	case 67: return 66;        // F9          0x42
	case 68: return 67;        // F10         0x43
	case 69:
		if (wParam == VK_PAUSE)
			return 72;         // PAUSE       0x48
		return 83;             // pad LOCK    0x53
	case 70: return 71;        // SCROLL      0x47
	case 71:
		if (wParam == VK_HOME)
			return 74;         // HOME        0x4A
		return 95;             // pad 7       0x5F
	case 72:
		if (wParam == VK_UP)
			return 82;         // UP          0x52
		return 96;             // pad 8       0x60
	case 73:
		if (wParam == VK_PRIOR)
			return 75;         // PAGEUP      0x4B
		return 97;             // pad 9       0x61
	case 74: return 86;        // pad -       0x56
	case 75:
		if (wParam == VK_LEFT)
			return 80;         // LEFT        0x50
		return 92;             // pad 4       0x5C
	case 76: return 93;        // pad 5       0x5D
	case 77:
		if (wParam == VK_RIGHT)
			return 79;         // RIGHT       0x4F
		return 94;             // pad 6       0x5E
	case 78: return 87;        // pad +       0x57
	case 79:
		if (wParam == VK_END)
			return 77;         // END         0x4D
		return 89;             // pad 1       0x59
	case 80:
		if (wParam == VK_DOWN)
			return 81;         // DOWN        0x51
		return 90;             // pad 2       0x5A
	case 81:
		if (wParam == VK_NEXT)
			return 78;         // PAGEDOWN    0x4E
		return 91;             // pad 3       0x5B
	case 82: return 73;        // INSERT      0x49
	case 83:
		if (wParam == VK_DELETE)
			return 76;         // DELETE F    0x4C
		return 99;             // pad DELETE  0x63
	case 84: return 0;
	case 85: return 0;
	case 86: return 100;       // |           0x64
	case 87: return 68;        // F11         0x44
	case 88: return 69;        // F12         0x45
	case 89: return 0;         // LWIN        0xE3
	case 90: return 0;         // RWIN        0xE7
	case 91: return 0;
	case 92: return 0;
	case 93: return 118;       // MENU        0x76
	}
	return key;
}

#endif /* OGLWND_H */
