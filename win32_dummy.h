/*
 *          Copyright 2022, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

static BOOL is_dummy(window_t *const window) {
	#ifdef UNICODE
	return (wcscmp(window[0].cls.lpszClassName, CLASS_NAME_DUMMY) == 0);
	#else
	return (strcmp(window[0].cls.lpszClassName, CLASS_NAME_DUMMY) == 0);
	#endif
}

static void dummy_class_register(void *const data, void **const err) {
	if (err[0] == NULL) {
		window_data_t *const wnd_data = (window_data_t*)data;
		wnd_data[0].wnd.cls.cbSize = sizeof(WNDCLASSEX);
		wnd_data[0].wnd.cls.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wnd_data[0].wnd.cls.lpfnWndProc = DefWindowProc;
		wnd_data[0].wnd.cls.cbClsExtra = 0;
		wnd_data[0].wnd.cls.cbWndExtra = 0;
		wnd_data[0].wnd.cls.hInstance = instance;
		wnd_data[0].wnd.cls.hIcon = NULL;
		wnd_data[0].wnd.cls.hCursor = NULL;
		wnd_data[0].wnd.cls.hbrBackground = NULL;
		wnd_data[0].wnd.cls.lpszMenuName = NULL;
		wnd_data[0].wnd.cls.lpszClassName = CLASS_NAME_DUMMY;
		wnd_data[0].wnd.cls.hIconSm = NULL;
		if (RegisterClassEx(&wnd_data[0].wnd.cls) == INVALID_ATOM) {
			err[0] = error_new(3, GetLastError(), NULL);
			// don't need to unregister class
			wnd_data[0].wnd.cls.lpszClassName = NULL;
			wnd_data[0].destroy(data, err);
		}
	}
}

static void dummy_window_create(void *const data, void **const err) {
	if (err[0] == NULL) {
		window_data_t *const wnd_data = (window_data_t*)data;
		wnd_data[0].wnd.hndl = CreateWindow(wnd_data[0].wnd.cls.lpszClassName, TEXT("Dummy"), WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, NULL, NULL, wnd_data[0].wnd.cls.hInstance, NULL);
		if (!wnd_data[0].wnd.hndl) {
			err[0] = error_new(4, GetLastError(), NULL);
			wnd_data[0].destroy(data, err);
		}
	}
}

static void dummy_context_create(void *const data, void **const err) {
	if (err[0] == NULL) {
		window_data_t *const wnd_data = (window_data_t*)data;
		wnd_data[0].wnd.ctx.dc = GetDC(wnd_data[0].wnd.hndl);
		if (wnd_data[0].wnd.ctx.dc) {
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
			pixelFormat = ChoosePixelFormat(wnd_data[0].wnd.ctx.dc, &pixelFormatDesc);
			if (pixelFormat) {
				if (SetPixelFormat(wnd_data[0].wnd.ctx.dc, pixelFormat, &pixelFormatDesc)) {
					wnd_data[0].wnd.ctx.rc = wglCreateContext(wnd_data[0].wnd.ctx.dc);
					if (!wnd_data[0].wnd.ctx.rc) {
						err[0] = error_new(8, GetLastError(), NULL);
						wnd_data[0].destroy(data, err);
					}
				} else {
					err[0] = error_new(7, GetLastError(), NULL);
					wnd_data[0].destroy(data, err);
				}
			} else {
				err[0] = error_new(6, GetLastError(), NULL);
				wnd_data[0].destroy(data, err);
			}
		} else {
			err[0] = error_new(5, ERROR_SUCCESS, NULL);
			wnd_data[0].destroy(data, err);
		}
	}
}

static void dummy_destroy(void *const data, void **const err) {
	window_data_t *const wnd_data = (window_data_t*)data;
	if (wnd_data[0].wnd.ctx.rc) {
		if (!wglDeleteContext(wnd_data[0].wnd.ctx.rc) && err[0] == NULL) {
			err[0] = error_new(10, GetLastError(), NULL);
		}
		wnd_data[0].wnd.ctx.rc = NULL;
	}
	if (wnd_data[0].wnd.ctx.dc) {
		ReleaseDC(wnd_data[0].wnd.hndl, wnd_data[0].wnd.ctx.dc);
		wnd_data[0].wnd.ctx.dc = NULL;
	}
	if (wnd_data[0].wnd.hndl) {
		if (!DestroyWindow(wnd_data[0].wnd.hndl) && err[0] == NULL) {
			err[0] = error_new(11, GetLastError(), NULL);
		}
		wnd_data[0].wnd.hndl = NULL;
	}
	if (wnd_data[0].wnd.cls.lpszClassName) {
		if (!UnregisterClass(wnd_data[0].wnd.cls.lpszClassName, wnd_data[0].wnd.cls.hInstance) && err[0] == NULL) {
			const DWORD err_win32 = GetLastError();
			if (err_win32 != ERROR_CLASS_HAS_WINDOWS)
				err[0] = error_new(12, err_win32, NULL);
		}
		wnd_data[0].wnd.cls.lpszClassName = NULL;
	}
}



/*
static void dummy_context_make_current(void *const data, void **const err) {
	if (err[0] == NULL) {
		if (!wglMakeCurrent(dummy.dc, dummy.rc)) {
			err[0] = new_error(8, GetLastError());
			wglDeleteContext(dummy.rc);
			ReleaseDC(dummy.hndl, dummy.dc);
			DestroyWindow(dummy.hndl);
			UnregisterClass(dummy.cls.lpszClassName, instance);
		}
	}
}


static void dummy_class_register(void *const data, void **const err) {
	if (err[0] == NULL) {
		window_data_t *const wnd_data = (window_data_t*)data;
		wnd_data[0].wnd.cls.cbSize = sizeof(WNDCLASSEX);
		wnd_data[0].wnd.cls.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wnd_data[0].wnd.cls.lpfnWndProc = DefWindowProc;
		wnd_data[0].wnd.cls.cbClsExtra = 0;
		wnd_data[0].wnd.cls.cbWndExtra = 0;
		wnd_data[0].wnd.cls.hInstance = instance;
		wnd_data[0].wnd.cls.hIcon = NULL;
		wnd_data[0].wnd.cls.hCursor = NULL;
		wnd_data[0].wnd.cls.hbrBackground = NULL;
		wnd_data[0].wnd.cls.lpszMenuName = NULL;
		wnd_data[0].wnd.cls.lpszClassName = CLASS_NAME_DUMMY;
		wnd_data[0].wnd.cls.hIconSm = NULL;
		if (RegisterClassEx(&wnd_data[0].wnd.cls) == INVALID_ATOM)
			err[0] = error_new(3, GetLastError(), NULL);
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
*/
