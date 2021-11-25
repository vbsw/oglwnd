/*
 *          Copyright 2021, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

#if defined(OGLWND_WIN32)

#include "oglwnd.h"
#include "win32.h"
#include "win32_init.h"

void oglwnd_window_destroy(void *const data) {
	if (data) {
		wnd_data_t *const wnd_data = (wnd_data_t*)data;
		window_release(&wnd_data->window);
		/* stop event queue thread */
		if (!is_class_registered(wnd_data->window.cls.lpszClassName))
			PostQuitMessage(0);
		free(wnd_data);
	}
}

/*
static DWORD get_style() {
	DWORD style;
	if (config.borderless)
		if (config.resizable)
			style = WS_POPUP;
		else
			style = WS_POPUP;
	else
		if (config.resizable)
			style = WS_OVERLAPPEDWINDOW;
		else
			style = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
	return style;
}
*/

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
		wnd_data_t **const wnd_data = (wnd_data_t**)data;
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
		wnd_data_t *const wnd_data = (wnd_data_t*)data;
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
	wnd_data_t *const wnd_data = (wnd_data_t*)data;
	if (!wglMakeCurrent(wnd_data->window.dc, wnd_data->window.rc)) {
		err[0] = 17;
		err_win32[0] = GetLastError();
	}
}

void oglwnd_ctx_release(void *const data, int *const err, oglwnd_ul_t *const err_win32, char **const err_str) {
	wnd_data_t *const wnd_data = (wnd_data_t*)data;
	if (!wglMakeCurrent(NULL, NULL)) {
		err[0] = 18;
		err_win32[0] = GetLastError();
	}
}

void oglwnd_swap_buffers(void *const data, int *const err, oglwnd_ul_t *const err_win32, char **const err_str) {
	wnd_data_t *const wnd_data = (wnd_data_t*)data;
	if (!SwapBuffers(wnd_data->window.dc)) {
		err[0] = 19;
		err_win32[0] = GetLastError();
	}
}

/* #if defined(_OGLWIN_WIN32) */
#endif
