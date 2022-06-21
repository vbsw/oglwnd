/*
 *          Copyright 2022, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

#if defined(OGLWND_WIN32)


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/GL.h>
#include "oglwnd.h"

/* Go functions can not be passed to c directly.            */
/* They can only be called from c.                          */
/* This code is an indirection to call Go callbacks.        */
/* _cgo_export.h is generated automatically by cgo.         */
#include "_cgo_export.h"

/* Exported functions from Go are:                          */
/* oglwndOnClose                                            */
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

// from wgl.h
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
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001

/* from wglext.h */
typedef BOOL(WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
typedef HGLRC(WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList);

#define CLASS_NAME TEXT("oglwnd_window")
#define CLASS_NAME_DUMMY TEXT("oglwnd_dummy")

typedef struct {
	int err_num;
	oglwnd_ul_t err_win32;
	char *err_str;
} error_t;

typedef struct {
	HDC dc;
	HGLRC rc;
} context_t;

typedef struct {
	WNDCLASSEX cls;
	HWND hndl;
	context_t ctx;
} window_t;

typedef struct {
	int x, y, width, height;
	int x_wnd, y_wnd, width_wnd, height_wnd;
} client_t;

typedef struct {
	int width_min, height_min, width_max, height_max;
	int borderless, dragable, fullscreen, resizable, locked;
	DWORD style;
} config_t;

typedef struct {
	window_t wnd;
	client_t client;
	LPTSTR title;
	config_t config;
	BOOL initialized;
	int go_obj_id;
	void *ext1;
	void *cust;
	void (*class_register)(void*, void**);
	void (*window_create)(void*, void**);
	void (*context_create)(void*, void**);
	BOOL (*message_proc)(void*, HWND, UINT, WPARAM, LPARAM, LRESULT*);
	void (*destroy)(void*, void**);
	void (*free)(void*, void**);
	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
} window_data_t;

static error_t err_no_mem = {1, ERROR_SUCCESS, NULL};
static HINSTANCE instance = NULL;

static void *error_new(const int err_num, const DWORD err_win32, char *const err_str) {
	error_t *const err = (error_t*)malloc(sizeof(error_t));
	if (err) {
		err->err_num = err_num;
		err->err_win32 = (oglwnd_ul_t)err_win32;
		err->err_str = err_str;
		return (void*)err;
	}
	if (err_str)
		free(err_str);
	return (void*)&err_no_mem;
}

static void module_init(void **const err) {
	if (instance == NULL) {
		if (err[0] == NULL) {
			instance = GetModuleHandle(NULL);
			if (!instance)
				err[0] = error_new(2, GetLastError(), NULL);
		}
	}
}

static BOOL is_class_registered(window_data_t *const wnd_data) {
	WNDCLASSEX wcx;
	if (GetClassInfoEx(wnd_data[0].wnd.cls.hInstance, wnd_data[0].wnd.cls.lpszClassName, &wcx))
		return TRUE;
	return FALSE;
}

static LPCTSTR ensure_title(window_data_t *const wnd_data) {
	if (wnd_data[0].title)
		return wnd_data[0].title;
	return TEXT("OpenGL");
}

static LRESULT CALLBACK windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
	if (message == WM_NCCREATE) {
		window_data_t *const wnd_data = (window_data_t*)(((CREATESTRUCT*)lParam)->lpCreateParams);
		if (wnd_data)
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)wnd_data);
		result = DefWindowProc(hWnd, message, wParam, lParam);
	} else {
		window_data_t *const wnd_data = (window_data_t*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (wnd_data == NULL || wnd_data[0].message_proc == NULL || wnd_data[0].message_proc((void*)wnd_data, hWnd, message, wParam, lParam, &result) == FALSE)
			result = DefWindowProc(hWnd, message, wParam, lParam);
	}
	return result;
}

static void window_style_update(window_data_t *const wnd_data) {
	if (wnd_data[0].config.borderless)
		if (wnd_data[0].config.resizable)
			wnd_data[0].config.style = WS_POPUP;
		else
			wnd_data[0].config.style = WS_POPUP;
	else
		if (wnd_data[0].config.resizable)
			wnd_data[0].config.style = WS_OVERLAPPEDWINDOW;
		else
			wnd_data[0].config.style = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
}

static void window_metrics(window_data_t *const wnd_data, int *const x, int *const y, int *const w, int *const h) {
	RECT rect = { wnd_data[0].client.x_wnd, wnd_data[0].client.y_wnd, wnd_data[0].client.x_wnd + wnd_data[0].client.width_wnd, wnd_data[0].client.y_wnd + wnd_data[0].client.height_wnd };
	AdjustWindowRect(&rect, wnd_data[0].config.style, FALSE);
	x[0] = rect.left;
	y[0] = rect.top;
	w[0] = rect.right - rect.left;
	h[0] = rect.bottom - rect.top;
}

static void monitor_metrics(HMONITOR const monitor, int *const x, int *const y, int *const w, int *const h) {
	MONITORINFO mi = { sizeof(mi) };
	GetMonitorInfo(monitor, &mi);
	x[0] = mi.rcMonitor.left;
	y[0] = mi.rcMonitor.top;
	w[0] = mi.rcMonitor.right - mi.rcMonitor.left;
	h[0] = mi.rcMonitor.bottom - mi.rcMonitor.top;
}

static void window_config_center(window_data_t *const wnd_data, const int monitor_id) {
	int wx, wy, ww, wh, mx, my, mw, mh;
	window_metrics(wnd_data, &wx, &wy, &ww, &wh);
	monitor_metrics(MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY), &mx, &my, &mw, &mh);
	wnd_data[0].client.x_wnd = mx + (mw - ww) / 2 + (wnd_data[0].client.x_wnd - wx);
	wnd_data[0].client.y_wnd = my + (mh - wh) / 2 + (wnd_data[0].client.y_wnd - wy);
}

#include "win32_dummy.h"
#include "win32_ogl30.h"
#include "win32_proc.h"

void oglwnd_error(void *const err, int *const err_num, oglwnd_ul_t *const err_win32, char **const err_str) {
	error_t *const error = (error_t*)err;
	err_num[0] = error->err_num;
	err_win32[0] = error->err_win32;
	err_str[0] = error->err_str;
}

void oglwnd_free(void *const data) {
	free(data);
}

void oglwnd_error_free(void *const data) {
	error_t *const err = (error_t*)data;
	if (err[0].err_str) {
		free(err[0].err_str);
		err[0].err_str = NULL;
	}
	if (err != &err_no_mem)
		free(data);
}

void oglwnd_window_allocate(void **const data, void **const err) {
	if (err[0] == NULL) {
		data[0] = malloc(sizeof(window_data_t));
		if (data[0])
			ZeroMemory(data[0], sizeof(window_data_t));
		else
			err[0] = error_new(1, ERROR_SUCCESS, NULL);
	}
}

void oglwnd_window_free(void *const data, void **const err) {
	window_data_t *const wnd_data = (window_data_t*)data;
	if (wnd_data[0].free)
		wnd_data[0].free(data, err);
	else
		free(data);
}

void oglwnd_window_init_dummy(void *const data, void **const err) {
	module_init(err);
	if (err[0] == NULL) {
		window_data_t *const wnd_data = (window_data_t*)data;
		wnd_data[0].class_register = dummy_class_register;
		wnd_data[0].window_create = dummy_window_create;
		wnd_data[0].context_create = dummy_context_create;
		wnd_data[0].destroy = dummy_destroy;
		wnd_data[0].free = NULL;
	}
}

void oglwnd_window_create(void *const data, void **const err) {
	if (err[0] == NULL) {
		window_data_t *const wnd_data = (window_data_t*)data;
		wnd_data[0].class_register(data, err);
		wnd_data[0].window_create(data, err);
		wnd_data[0].context_create(data, err);
	}
}

void oglwnd_window_destroy(void *const data, void **const err) {
	if (err[0] == NULL) {
		window_data_t *const wnd_data = (window_data_t*)data;
		wnd_data[0].destroy(data, err);
	}
}

void oglwnd_window_init_opengl30(void *const data, const int go_obj, const int x, const int y, const int w, const int h, const int wn, const int hn,
	const int wx, const int hx, const int b, const int d, const int r, const int f, const int l, const int c, void **const err) {
	module_init(err);
	if (err[0] == NULL) {
		window_data_t *const wnd_data = (window_data_t*)data;
		wnd_data[0].class_register = ogl30_class_register;
		wnd_data[0].window_create = ogl30_window_create;
		wnd_data[0].context_create = ogl30_context_create;
		wnd_data[0].message_proc = ogl30_message_proc;
		wnd_data[0].destroy = ogl30_destroy;
		wnd_data[0].free = ogl30_free;
		wnd_data[0].go_obj_id = go_obj;
		wnd_data[0].client.x_wnd = x;
		wnd_data[0].client.y_wnd = y;
		wnd_data[0].client.width_wnd = w;
		wnd_data[0].client.height_wnd = h;
		wnd_data[0].config.width_min = wn;
		wnd_data[0].config.height_min = hn;
		wnd_data[0].config.width_max = wx;
		wnd_data[0].config.height_max = hx;
		wnd_data[0].config.borderless = b;
		wnd_data[0].config.dragable = d;
		wnd_data[0].config.fullscreen = f;
		wnd_data[0].config.resizable = r;
		wnd_data[0].config.locked = l;
		window_style_update(wnd_data);
		if (c)
			window_config_center(wnd_data, 0);
	}
}

void oglwnd_window_set_wgl_functions(void *const data, void *const cpf, void *const cca) {
	window_data_t *const wnd_data = (window_data_t*)data;
	wnd_data[0].wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)cpf;
	wnd_data[0].wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)cca;
}

void oglwnd_context_make_current(void *const data, void **const err) {
	if (err[0] == NULL) {
		context_t *const ctx = (context_t*)data;
		if (!wglMakeCurrent(ctx[0].dc, ctx[0].rc))
			err[0] = error_new(56, GetLastError(), NULL);
	}
}

void oglwnd_context_release(void *const data, void **const err) {
	if (err[0] == NULL) {
		context_t *const ctx = (context_t*)data;
		if (ctx[0].rc && ctx[0].rc == wglGetCurrentContext())
			if (!wglMakeCurrent(NULL, NULL))
				err[0] = error_new(57, GetLastError(), NULL);
	}
}

void oglwnd_context_swap_buffers(void *const data, void **const err) {
	if (err[0] == NULL) {
		context_t *const ctx = (context_t*)data;
		if (!SwapBuffers(ctx[0].dc))
			err[0] = error_new(61, GetLastError(), NULL);
	}
}

void oglwnd_window_context(void *const data, void **const ctx) {
	window_data_t *const wnd_data = (window_data_t*)data;
	ctx[0] = (void*)&wnd_data[0].wnd.ctx;
}

void oglwnd_window_show(void *const data, void **const err) {
	window_data_t *const wnd_data = (window_data_t*)data;
	ShowWindow(wnd_data[0].wnd.hndl, SW_SHOWDEFAULT);
}

void oglwnd_window_props(void *const data, int *const x, int *const y, int *const w, int *const h, int *const wn, int *const hn, int *const wx, int *const hx, int *const b, int *const d, int *const r, int *const f, int *const l) {
	window_data_t *const wnd_data = (window_data_t*)data;
	x[0] = wnd_data[0].client.x;
	y[0] = wnd_data[0].client.y;
	w[0] = wnd_data[0].client.width;
	h[0] = wnd_data[0].client.height;
/*
	wn[0] = wnd_data[0].config.width_min;
	hn[0] = wnd_data[0].config.height_min;
	wx[0] = wnd_data[0].config.width_max;
	hx[0] = wnd_data[0].config.height_max;
	b[0] = wnd_data[0].config.borderless;
	d[0] = wnd_data[0].config.dragable;
	r[0] = wnd_data[0].config.resizable;
	f[0] = wnd_data[0].config.fullscreen;
	l[0] = wnd_data[0].config.locked;
*/
}

int oglwnd_window_funcs_avail(void *const data) {
	window_data_t *const wnd_data = (window_data_t*)data;
	if (wnd_data[0].class_register && wnd_data[0].window_create && wnd_data[0].context_create && wnd_data[0].destroy)
		return 1;
	return 0;
}

int oglwnd_window_dt_func_avail(void *const data) {
	window_data_t *const wnd_data = (window_data_t*)data;
	if (wnd_data[0].destroy)
		return 1;
	return 0;
}

void oglwnd_process_events() {
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


/*
#include "oglwnd.h"
#include "win32.h"
#include "win32_init.h"

void oglwnd_window_destroy(void *const data) {
	if (data) {
		window_data_t *const wnd_data = (window_data_t*)data;
		window_release(&wnd_data->window);
		// stop event queue thread
		if (!is_class_registered(wnd_data->window.cls.lpszClassName))
			PostQuitMessage(0);
		free(wnd_data);
	}
}

void oglwnd_free_mem(void *const mem) {
	free(mem);
}

void oglwnd_process_events() {
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void oglwnd_process_events_blocking() {
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void oglwnd_init(int *const err, oglwnd_ul_t *const err_win32) {
	int error = 0;
	oglwnd_ul_t error_win32 = 0;
	module_init(&error, &error_win32);
	dummy_class_init(&error, &error_win32);
	dummy_window_create(&error, &error_win32);
	dummy_context_init(&error, &error_win32);
	wgl_functions_init(&error, &error_win32);
	monitor_init(&error, &error_win32);
	window_release(&dummy);
	err[0] = error;
	err_win32[0] = error_win32;
}

void oglwnd_window_new(void **const data, void *const go_obj, const int x, const int y, const int w, const int h, const int wn, const int hn,
	const int wx, const int hx, const int b, const int d, const int r, const int f, const int l, const int c, int *const err, oglwnd_ul_t *const err_win32, char **const err_str) {
	if (err[0] == 0) {
		int error = 0;
		oglwnd_ul_t error_win32 = 0;
		char *error_str = NULL;
		window_data_t **const wnd_data = (window_data_t**)data;
		window_alloc(wnd_data, go_obj, &error, &error_win32, &error_str);
		if (error == 0) {
			config_t *const config = &wnd_data[0]->config;
			config->x = x;
			config->y = y;
			config->width = w;
			config->height = h;
			config->width_min = wn;
			config->height_min = hn;
			config->width_max = wx;
			config->height_max = hx;
			config->centered = c;
			config->borderless = b;
			config->dragable = d;
			config->resizable = r;
			config->fullscreen = f;
			config->locked = l;
			config_ensure(config);
		} else {
			oglwnd_window_destroy(data[0]);
			err[0] = error;
			err_win32[0] = error_win32;
			err_str[0] = error_str;
		}
	}
}

void oglwnd_window_init(void *const data, int *const err, oglwnd_ul_t *const err_win32, char **const err_str) {
	if (err[0] == 0) {
		int error = 0;
		oglwnd_ul_t error_win32 = 0;
		char *error_str = NULL;
		window_data_t *const wnd_data = (window_data_t*)data;
		window_class_init(wnd_data, &error, &error_win32, &error_str);
		window_create(wnd_data, &error, &error_win32, &error_str);
		window_context_init(wnd_data, &error, &error_win32, &error_str);
		if (error) {
			oglwnd_window_destroy(data);
			err[0] = error;
			err_win32[0] = error_win32;
			err_str[0] = error_str;
		}
	}
}

void oglwnd_ctx_make_current(void *const data, int *const err, oglwnd_ul_t *const err_win32, char **const err_str) {
	window_data_t *const wnd_data = (window_data_t*)data;
	if (!wglMakeCurrent(wnd_data->window.dc, wnd_data->window.rc)) {
		err[0] = 17;
		err_win32[0] = GetLastError();
	}
}

void oglwnd_ctx_release(void *const data, int *const err, oglwnd_ul_t *const err_win32, char **const err_str) {
	window_data_t *const wnd_data = (window_data_t*)data;
	if (!wglMakeCurrent(NULL, NULL)) {
		err[0] = 18;
		err_win32[0] = GetLastError();
	}
}

void oglwnd_swap_buffers(void *const data, int *const err, oglwnd_ul_t *const err_win32, char **const err_str) {
	window_data_t *const wnd_data = (window_data_t*)data;
	if (!SwapBuffers(wnd_data->window.dc)) {
		err[0] = 19;
		err_win32[0] = GetLastError();
	}
}
*/

/* #if defined(_OGLWIN_WIN32) */
#endif
