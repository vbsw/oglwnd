/*
 *          Copyright 2021, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

static void module_init(int *const err, oglwnd_ul_t *const err_win32) {
	if (instance == NULL) {
		instance = GetModuleHandle(NULL);
		if (!instance) {
			err[0] = 1;
			err_win32[0] = GetLastError();
		}
	}
}

static void dummy_class_init(int *const err, oglwnd_ul_t *const err_win32) {
	if (err[0] == 0 && (!wglChoosePixelFormatARB || !wglCreateContextAttribsARB)) {
		dummy.cls.cbSize = sizeof(WNDCLASSEX);
		dummy.cls.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		dummy.cls.lpfnWndProc = DefWindowProc;
		dummy.cls.cbClsExtra = 0;
		dummy.cls.cbWndExtra = 0;
		dummy.cls.hInstance = instance;
		dummy.cls.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		dummy.cls.hCursor = cursor.arrow;
		dummy.cls.hbrBackground = NULL;
		dummy.cls.lpszMenuName = NULL;
		dummy.cls.lpszClassName = DUMMY_CLASS_NAME;
		dummy.cls.hIconSm = NULL;
		if (RegisterClassEx(&dummy.cls) == INVALID_ATOM) {
			err[0] = 2;
			err_win32[0] = GetLastError();
		}
	}
}

static void dummy_window_create(int *const err, oglwnd_ul_t *const err_win32) {
	if (err[0] == 0 && (!wglChoosePixelFormatARB || !wglCreateContextAttribsARB)) {
		dummy.hndl = CreateWindow(dummy.cls.lpszClassName, TEXT("Dummy"), WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, NULL, NULL, instance, NULL);
		if (!dummy.hndl) {
			err[0] = 3;
			err_win32[0] = GetLastError();
		}
	}
}

static void dummy_context_init(int *const err, oglwnd_ul_t *const err_win32) {
	if (err[0] == 0 && (!wglChoosePixelFormatARB || !wglCreateContextAttribsARB)) {
		dummy.dc = GetDC(dummy.hndl);
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
						err[0] = 7;
						err_win32[0] = GetLastError();
					}
				} else {
					err[0] = 6;
					err_win32[0] = GetLastError();
				}
			} else {
				err[0] = 5;
				err_win32[0] = GetLastError();
			}
		} else {
			err[0] = 4;
		}
	}
}

static void wgl_functions_init(int *const err, oglwnd_ul_t *const err_win32) {
	if (err[0] == 0 && (!wglChoosePixelFormatARB || !wglCreateContextAttribsARB)) {
		if (wglMakeCurrent(dummy.dc, dummy.rc)) {
			wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
			if (wglChoosePixelFormatARB) {
				wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
				if (!wglCreateContextAttribsARB) {
					err[0] = 10;
					err_win32[0] = GetLastError();
				}
			} else {
				err[0] = 9;
				err_win32[0] = GetLastError();
			}
		} else {
			err[0] = 8;
			err_win32[0] = GetLastError();
		}
	}
}

static void monitor_init(int *const err, oglwnd_ul_t *const err_win32) {
	if (err[0] == 0) {
		if (monitor.hndl == NULL) {
			monitor.hndl = MonitorFromWindow(dummy.hndl, MONITOR_DEFAULTTONEAREST);
			if (monitor.hndl == NULL)
				monitor.hndl = MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY);
		}
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(monitor.hndl, &mi);
		monitor.x = mi.rcMonitor.left;
		monitor.y = mi.rcMonitor.top;
		monitor.width = mi.rcMonitor.right - mi.rcMonitor.left;
		monitor.height = mi.rcMonitor.bottom - mi.rcMonitor.top;
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

static void window_alloc(wnd_data_t **const data, void *go_obj, int *const err, oglwnd_ul_t *const err_win32, char **const err_str) {
	if (err[0] == 0) {
		data[0] = (wnd_data_t*)malloc(sizeof(wnd_data_t));
		ZeroMemory(data[0], sizeof(wnd_data_t));
		data[0]->go_obj = go_obj;
	}
}

static void window_class_init(wnd_data_t *const data, int *const err, oglwnd_ul_t *const err_win32, char **const err_str) {
	if (err[0] == 0) {
		data->window.cls.cbSize = sizeof(WNDCLASSEX);
		data->window.cls.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		data->window.cls.lpfnWndProc = DefWindowProc;
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
			err[0] = 11;
			err_win32[0] = GetLastError();
		}
	}
}

void window_create(wnd_data_t *const data, int *const err, oglwnd_ul_t *const err_win32, char **const err_str) {
	if (err[0] == 0) {
		const DWORD style = WS_OVERLAPPEDWINDOW;
		data->window.hndl = CreateWindow(data->window.cls.lpszClassName, TEXT("OpenGL"), style, 10, 10, 640, 480, NULL, NULL, data->window.cls.hInstance, (void*)data);
		if (!data->window.hndl) {
			err[0] = 12;
			err_win32[0] = GetLastError();
		}
	}
}

void window_context_init(wnd_data_t *const data, int *const err, oglwnd_ul_t *const err_win32, char **const err_str) {
	if (err[0] == 0) {
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
						err[0] = 16;
						err_win32[0] = GetLastError();
					}
				} else {
					err[0] = 15;
					err_win32[0] = GetLastError();
				}
			} else {
				err[0] = 14;
				err_win32[0] = GetLastError();
			}
		} else {
			err[0] = 13;
		}
	}
}
