/*
 *          Copyright 2021, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

#if defined(_OGLWND_WIN32)

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
		window_t *dummy = NULL;
		builder_t *const bldr = (builder_t*)builder;
		bldr->init_module(&instance, err_num, err_str_extra);
		bldr->new_dummy(&dummy, instance, err_num, err_str_extra);
		bldr->init_dummy_class(dummy, instance, err_num, err_str_extra);
		bldr->init_dummy_window(dummy, err_num, err_str_extra);
		bldr->init_dummy_context(dummy, err_num, err_str_extra);
		bldr->init_wgl(dummy, &wglChoosePixelFormatARB, &wglCreateContextAttribsARB, err_num, err_str_extra);
		bldr->destroy_dummy(dummy);
	}
}

void oglwnd_destroy() {
}

void new_window_impl(window_t **const window, HINSTANCE const instance, int *const err_num, char **const err_str_extra) {
	window[0] = (window_t*)malloc(sizeof(window_t));
	ZeroMemory(window[0], sizeof(window_t));
}

void init_class_impl(window_t *const window, HINSTANCE const instance, int *const err_num, char **const err_str_extra) {
	LPTSTR const class_name = new_class_name();
	window->cls.cbSize = sizeof(WNDCLASSEX);
	window->cls.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	window->cls.lpfnWndProc = windowProc;
	window->cls.cbClsExtra = 0;
	window->cls.cbWndExtra = 0;
	window->cls.hInstance = instance;
	window->cls.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	window->cls.hCursor = cursor.cust;
	window->cls.hbrBackground = NULL;
	window->cls.lpszMenuName = NULL;
	window->cls.lpszClassName = class_name;
	window->cls.hIconSm = NULL;
	if (RegisterClassEx(&window->cls) == INVALID_ATOM) {
		if (class_name)
			free(class_name);
		err_num[0] = 11;
	}
}

void init_window_impl() {
}

void init_context_impl() {
}

/* #if defined(_OGLWIN_WIN32) */
#endif
