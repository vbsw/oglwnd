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

static TCHAR str_buffer_50[50];
static int class_counter = 0;

static LPTSTR new_class_name() {
	#ifdef UNICODE
	const int length = swprintf(str_buffer_50, 50, L"oglwnd_cls_%i", class_counter++);
	#else
	const int length = snprintf(str_buffer_50, 50, "oglwnd_cls_%i", class_counter++);
	#endif
	if (length > 0) {
		const size_t size = sizeof(TCHAR) * (size_t)(length + 1);
		LPTSTR const class_name = (LPTSTR)malloc(size);
		memcpy(class_name, str_buffer_50, size);
		return class_name;
	}
	return NULL;
}

static LRESULT CALLBACK windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	return DefWindowProc(hWnd, message, wParam, lParam);
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

void oglwnd_destroy() {
}

void new_window_impl(window_data_t **const data, HINSTANCE const instance, int *const err_num, char **const err_str_extra) {
	data[0] = (window_data_t*)malloc(sizeof(window_data_t));
	ZeroMemory(data[0], sizeof(window_data_t));
}

void init_class_impl(window_data_t *const data, HINSTANCE const instance, int *const err_num, char **const err_str_extra) {
	if (err_num[0] == 0) {
		LPTSTR const class_name = new_class_name();
		data->window.cls.cbSize = sizeof(WNDCLASSEX);
		data->window.cls.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		data->window.cls.lpfnWndProc = windowProc;
		data->window.cls.cbClsExtra = 0;
		data->window.cls.cbWndExtra = 0;
		data->window.cls.hInstance = instance;
		data->window.cls.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		data->window.cls.hCursor = cursor.cust;
		data->window.cls.hbrBackground = NULL;
		data->window.cls.lpszMenuName = NULL;
		data->window.cls.lpszClassName = class_name;
		data->window.cls.hIconSm = NULL;
		if (RegisterClassEx(&data->window.cls) == INVALID_ATOM) {
			if (class_name)
				free(class_name);
			err_num[0] = 11;
		}
	}
}

void init_window_impl(window_data_t *const data, int *const err_num, char **const err_str_extra) {
	if (err_num[0] == 0) {
		const DWORD style = WS_OVERLAPPEDWINDOW;
		data->window.hndl = CreateWindow(data->window.cls.lpszClassName, TEXT("OpenGL"), style, 10, 10, 640, 480, NULL, NULL, data->window.cls.hInstance, NULL);
		if (!data->window.hndl)
			err_num[0] = 12;
	}
}

void init_context_impl(window_data_t *const data, int *const err_num, char **const err_str_extra) {
	if (err_num[0] == 0) {
		data->window.dc = GetDC(data->window.hndl);
		if (data->window.dc) {
			int pixelFormat;
			BOOL status;
			UINT numFormats;
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
					if (data->window.rc) {
						if (!wglMakeCurrent(data->window.dc, data->window.rc)) {
							err_num[0] = 17;
						}
					} else {
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
