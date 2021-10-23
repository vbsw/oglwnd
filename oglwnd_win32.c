/*
 *          Copyright 2021, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

#if defined(_OGLWND_WIN32)

#include "oglwnd_win32.h"

static DWORD get_style_from_config() {
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

static void init_module_handle(int *const err) {
	if (instance == NULL) {
		instance = GetModuleHandle(NULL);
		if (instance == NULL)
			*err = 1;
	}
}

static void init_dummy_class(int *const err) {
	if (*err == 0 && (!wglChoosePixelFormatARB || !wglCreateContextAttribsARB)) {
		dummy.cls.cbSize = sizeof(WNDCLASSEX);
		dummy.cls.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		dummy.cls.lpfnWndProc = DefWindowProc;
		dummy.cls.cbClsExtra = 0;
		dummy.cls.cbWndExtra = 0;
		dummy.cls.hInstance = instance;
		dummy.cls.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		dummy.cls.hCursor = LoadCursor(NULL, IDC_ARROW);
		dummy.cls.hbrBackground = NULL;
		dummy.cls.lpszMenuName = NULL;
		dummy.cls.lpszClassName = DUMMY_CLASS_NAME;
		dummy.cls.hIconSm = NULL;
		if (RegisterClassEx(&dummy.cls) == INVALID_ATOM)
			*err = 2;
	}
}

static void init_dummy_window(int *const err) {
	if (*err == 0 && (!wglChoosePixelFormatARB || !wglCreateContextAttribsARB)) {
		dummy.hndl = CreateWindow(DUMMY_CLASS_NAME, TEXT("Dummy"), WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, NULL, NULL, instance, NULL);
		if (!dummy.hndl)
			*err = 3;
	}
}

static void init_dummy_context(int *const err) {
	if (*err == 0 && (!wglChoosePixelFormatARB || !wglCreateContextAttribsARB)) {
		dummy.dc = GetDC(window.hndl);
		if (dummy.dc) {
			int pixelFormat;
			PIXELFORMATDESCRIPTOR pixelFormatDesc;
			ZeroMemory(&pixelFormatDesc, sizeof(PIXELFORMATDESCRIPTOR));
			pixelFormatDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
			pixelFormatDesc.nVersion = 1;
			pixelFormatDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
			pixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
			pixelFormatDesc.cColorBits = 32;
			pixelFormatDesc.cAlphaBits = 8;
			pixelFormatDesc.cDepthBits = 24;
			pixelFormat = ChoosePixelFormat(dummy.dc, &pixelFormatDesc);
			if (pixelFormat) {
				if (SetPixelFormat(dummy.dc, pixelFormat, &pixelFormatDesc)) {
					dummy.rc = wglCreateContext(dummy.dc);
					if (!dummy.rc) {
						*err = 7;
					}
				} else {
					*err = 6;
				}
			} else {
				*err = 5;
			}
		} else {
			*err = 4;
		}
	}
}

static void init_wgl_functions(int *const err) {
	if (*err == 0 && (!wglChoosePixelFormatARB || !wglCreateContextAttribsARB)) {
		if (wglMakeCurrent(dummy.dc, dummy.rc)) {
			wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
			if (wglChoosePixelFormatARB) {
				wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
				if (!wglCreateContextAttribsARB) {
					*err = 10;
				}
			} else {
				*err = 9;
			}
		} else {
			*err = 8;
		}
	}
}

static void destroy_window(window_t *const wnd) {
	if (wnd->rc) {
		wglMakeCurrent(wnd->dc, NULL);
		wglDeleteContext(wnd->rc);
		wnd->rc = NULL;
	}
	if (wnd->dc) {
		ReleaseDC(wnd->hndl, wnd->dc);
		wnd->dc = NULL;
	}
	if (wnd->hndl) {
		DestroyWindow(wnd->hndl);
		wnd->hndl = NULL;
	}
	if (wnd->cls.lpszClassName)
		UnregisterClass(wnd->cls.lpszClassName, instance);
}

static LRESULT CALLBACK windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
	switch (message) {
	case WM_CLOSE:
		result = DefWindowProc(hWnd, message, wParam, lParam);
		goOnClose();
		break;
	case WM_DESTROY:
		/* stop event queue thread */
        PostQuitMessage(0);
        break;
	default:
		result = DefWindowProc(hWnd, message, wParam, lParam);
	}
	return result;
}

static void init_class(int *const err) {
	window.cls.cbSize = sizeof(WNDCLASSEX);
	window.cls.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	window.cls.lpfnWndProc = windowProc;
	window.cls.cbClsExtra = 0;
	window.cls.cbWndExtra = 0;
	window.cls.hInstance = instance;
	window.cls.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	window.cls.hCursor = LoadCursor(NULL, IDC_ARROW);
	window.cls.hbrBackground = NULL;
	window.cls.lpszMenuName = NULL;
	window.cls.lpszClassName = CLASS_NAME;
	window.cls.hIconSm = NULL;
	if (RegisterClassEx(&window.cls) == INVALID_ATOM) {
		*err = 11;
	}
}

static void init_window(int *const err) {
	if (*err == 0) {
		const DWORD style = get_style_from_config();
		window.hndl = CreateWindow(CLASS_NAME, TEXT("OpenGL"), style, 0, 0, 640, 480, NULL, NULL, instance, NULL);
		if (!window.hndl)
			*err = 12;
	}
}

static void init_context(int *const err) {
	if (*err == 0) {
		window.dc = GetDC(window.hndl);
		if (window.dc) {
			int pixelFormat;
			BOOL status;
			UINT numFormats;
			const int pixelAttribs[] = {
				WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
				WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
				WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
				/* WGL_SWAP_EXCHANGE_ARB causes problems in fullscreen */
				WGL_SWAP_METHOD_ARB, WGL_SWAP_COPY_ARB,
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
			status = wglChoosePixelFormatARB(window.dc, pixelAttribs, NULL, 1, &pixelFormat, &numFormats);
			if (status && numFormats) {
				PIXELFORMATDESCRIPTOR pfd;
				memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
				DescribePixelFormat(window.dc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
				if (SetPixelFormat(window.dc, pixelFormat, &pfd)) {
					window.rc = wglCreateContextAttribsARB(window.dc, 0, contextAttributes);
					if (window.rc) {
						if (!wglMakeCurrent(window.dc, window.rc)) {
							*err = 17;
						}
					} else {
						*err = 16;
					}
				} else {
					*err = 15;
				}
			} else {
				*err = 14;
			}
		} else {
			*err = 13;
		}
	}
}

void oglwnd_init(int x, int y, int w, int h, int wMin, int hMin, int wMax, int hMax, int c, int b, int d, int r, int f, int *err) {
	int error = 0;
	init_module_handle(&error);
	init_dummy_class(&error);
	init_dummy_window(&error);
	init_dummy_context(&error);
	init_wgl_functions(&error);
	destroy_window(&dummy);
	if (!error) {
		config.x = x;
		config.y = y;
		config.width = w;
		config.height = h;
		config.widthMin = wMin;
		config.heightMin = hMin;
		config.widthMax = wMax;
		config.heightMax = hMax;
		config.centered = c;
		config.borderless = b;
		config.dragable = d;
		config.resizable = r;
		config.fullscreen = f;
		init_class(&error);
		init_window(&error);
		init_context(&error);
		if (error) {
			destroy_window(&window);
		}
	}
	*err = error;
}

void oglwnd_show() {
	ShowWindow(window.hndl, SW_SHOWDEFAULT);
	running = TRUE;
}

void oglwnd_main_loop() {
	MSG msg;
	while (running && PeekMessage(&msg, window.hndl, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (running) {
		goOnFirstUpdate();
		while (running) {
			while (running && PeekMessage(&msg, window.hndl, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (running)
				goOnUpdate();
		}
	}
}

void oglwnd_destroy() {
	destroy_window(&window);
}

void oglwnd_stop() {
	running = FALSE;
}

/* #if defined(_OGLWIN_WIN32) */
#endif
