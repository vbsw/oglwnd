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

void oglwnd_window_new(void **const data, void *const go_obj, int *const err, oglwnd_ul_t *const err_win32, char **const err_str) {
	int error = 0;
	oglwnd_ul_t error_win32 = 0;
	char *error_str = NULL;
	wnd_data_t **const wnd_data = (wnd_data_t**)data;
	window_alloc(wnd_data, go_obj, &error, &error_win32, &error_str);
	window_class_init(wnd_data[0], &error, &error_win32, &error_str);
	window_create(wnd_data[0], &error, &error_win32, &error_str);
	window_context_init(wnd_data[0], &error, &error_win32, &error_str);
	if (error) {
		oglwnd_window_destroy(data[0]);
		err[0] = error;
		err_win32[0] = error_win32;
		err_str[0] = error_str;
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
