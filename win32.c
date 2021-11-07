/*
 *          Copyright 2021, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

#if defined(OGLWND_WIN32)

#include "win32.h"
#include "win32_dummy.h"
#include "win32_builder.h"

static LRESULT CALLBACK windowProc(HWND const hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam) {
	LRESULT result = 0;
	if (message == WM_NCCREATE) {
		window_data_t *const data = (window_data_t*)(((CREATESTRUCT*)lParam)->lpCreateParams);
		if (data)
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)data);
		result = DefWindowProc(hWnd, message, wParam, lParam);
	} else {
		window_data_t *const data = (window_data_t*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		switch (message) {
		case WM_CLOSE:
			goOnCloseRequest(data->go_data);
			break;
		default:
			result = DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	return result;
}

void oglwnd_init(void *const builder, int *const err_num, char **const err_str_extra) {
	if (err_num[0] == 0) {
		dummy_data_t *data = NULL;
		builder_t *const bldr = (builder_t*)builder;
		bldr->init_module(&instance, err_num, err_str_extra);
		bldr->new_dummy(&data, instance, err_num, err_str_extra);
		bldr->init_dummy_class(data, instance, err_num, err_str_extra);
		bldr->init_dummy_window(data, err_num, err_str_extra);
		bldr->init_dummy_context(data, err_num, err_str_extra);
		bldr->init_wgl(data, &wglChoosePixelFormatARB, &wglCreateContextAttribsARB, err_num, err_str_extra);
		bldr->destroy_dummy(data);
	}
}

void oglwnd_new_window(void *const builder, void **const data, void *const go_data, int *const err_num, char **const err_str_extra) {
	builder_t *const bldr = (builder_t*)builder;
	window_data_t **const window_data = (window_data_t**)data;
	bldr->new_window(window_data, instance, go_data, err_num, err_str_extra);
	bldr->init_class(window_data[0], instance, err_num, err_str_extra);
	bldr->init_window(window_data[0], err_num, err_str_extra);
	bldr->init_context(window_data[0], err_num, err_str_extra);
}

void oglwnd_destroy_window(void *const data) {
	if (data) {
		window_data_t *const window_data = (window_data_t*)data;
		if (window_data->window.rc) {
			wglMakeCurrent(window_data->window.dc, NULL);
			wglDeleteContext(window_data->window.rc);
		}
		if (window_data->window.dc) {
			ReleaseDC(window_data->window.hndl, window_data->window.dc);
		}
		if (window_data->window.hndl) {
			DestroyWindow(window_data->window.hndl);
		}
		UnregisterClass(window_data->window.cls.lpszClassName, window_data->window.cls.hInstance);
		/* stop event queue thread */
		if (!is_class_registered(window_data->window.cls.lpszClassName))
			PostQuitMessage(0);
		free(window_data);
	}
}

void oglwnd_destroy() {
}

void oglwnd_free_mem(void *const mem) {
	if (mem)
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

void oglwnd_process_window_events(void *const data) {
	MSG msg;
	window_data_t *const window_data = (window_data_t*)data;
	while (PeekMessage(&msg, window_data->window.hndl, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void oglwnd_process_window_events_blocking(void *const data) {
	MSG msg;
	window_data_t *const window_data = (window_data_t*)data;
	while (GetMessage(&msg, window_data->window.hndl, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void oglwnd_show(void *const data) {
	window_data_t *const window_data = (window_data_t*)data;
	ShowWindow(window_data->window.hndl, SW_SHOWDEFAULT);
}

void oglwnd_set_title(void *const data, char *const title) {
	BOOL converted = FALSE;
	window_data_t *const window_data = (window_data_t*)data;
	LPCTSTR const title_conv = convert_str(title, &converted);
	SetWindowText(window_data->window.hndl, title_conv);
	if (converted)
		free((void*)title_conv);
}

void new_window_impl(window_data_t **const data, HINSTANCE const instance, void *go_data, int *const err_num, char **const err_str_extra) {
	data[0] = (window_data_t*)malloc(sizeof(window_data_t));
	ZeroMemory(data[0], sizeof(window_data_t));
	data[0]->go_data = go_data;
}

void init_class_impl(window_data_t *const data, HINSTANCE const instance, int *const err_num, char **const err_str_extra) {
	if (err_num[0] == 0) {
		data->window.cls.cbSize = sizeof(WNDCLASSEX);
		data->window.cls.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		data->window.cls.lpfnWndProc = windowProc;
		data->window.cls.cbClsExtra = 0;
		data->window.cls.cbWndExtra = 0;
		data->window.cls.hInstance = instance;
		data->window.cls.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		data->window.cls.hCursor = LoadCursor(NULL, IDC_ARROW);
		data->window.cls.hbrBackground = NULL;
		data->window.cls.lpszMenuName = NULL;
		data->window.cls.lpszClassName = CLASS_NAME;
		data->window.cls.hIconSm = NULL;
		if (!is_class_registered(data->window.cls.lpszClassName) && RegisterClassEx(&data->window.cls) == INVALID_ATOM) {
			err_num[0] = 11;
		}
	}
}

void init_window_impl(window_data_t *const data, int *const err_num, char **const err_str_extra) {
	if (err_num[0] == 0) {
		const DWORD style = WS_OVERLAPPEDWINDOW;
		data->window.hndl = CreateWindow(data->window.cls.lpszClassName, TEXT("OpenGL"), style, 10, 10, 640, 480, NULL, NULL, data->window.cls.hInstance, (void*)data);
		if (!data->window.hndl)
			err_num[0] = 12;
	}
}

void init_context_impl(window_data_t *const data, int *const err_num, char **const err_str_extra) {
	if (err_num[0] == 0) {
		data->window.dc = GetDC(data->window.hndl);
		if (data->window.dc) {
			int pixelFormat;
			BOOL status = FALSE;
			UINT numFormats = 0;
			const int pixelAttribs[] = {
				WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
				WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
				WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
				/* WGL_SWAP_COPY_ARB has update problems in fullscreen */
				/* WGL_SWAP_EXCHANGE_ARB has problems with start menu in fullscreen */
				WGL_SWAP_METHOD_ARB, WGL_SWAP_EXCHANGE_ARB,
				WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
				WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
				WGL_COLOR_BITS_ARB, 32,
				WGL_ALPHA_BITS_ARB, 8,
				WGL_DEPTH_BITS_ARB, 24,
				0
			};
			int  contextAttributes[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
				WGL_CONTEXT_MINOR_VERSION_ARB, 0,
				WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
				0
			};
			status = wglChoosePixelFormatARB(data->window.dc, pixelAttribs, NULL, 1, &pixelFormat, &numFormats);
			if (status && numFormats) {
				PIXELFORMATDESCRIPTOR pfd;
				memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
				DescribePixelFormat(data->window.dc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
				if (SetPixelFormat(data->window.dc, pixelFormat, &pfd)) {
					data->window.rc = wglCreateContextAttribsARB(data->window.dc, 0, contextAttributes);
					if (!data->window.rc) {
						err_num[0] = 16;
					}
				} else {
					err_num[0] = 15;
				}
			} else {
				err_num[0] = 14;
			}
		} else {
			err_num[0] = 13;
		}
	}
}

/* #if defined(_OGLWIN_WIN32) */
#endif
