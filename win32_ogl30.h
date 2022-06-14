/*
 *          Copyright 2022, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

static void ogl30_class_register(void *const data, void **const err) {
	if (err[0] == NULL) {
		window_data_t *const wnd_data = (window_data_t*)data;
		wnd_data[0].wnd.cls.cbSize = sizeof(WNDCLASSEX);
		wnd_data[0].wnd.cls.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wnd_data[0].wnd.cls.lpfnWndProc = DefWindowProc;
		wnd_data[0].wnd.cls.cbClsExtra = 0;
		wnd_data[0].wnd.cls.cbWndExtra = 0;
		wnd_data[0].wnd.cls.hInstance = instance;
		wnd_data[0].wnd.cls.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wnd_data[0].wnd.cls.hCursor = LoadCursor(NULL, IDC_ARROW);
		wnd_data[0].wnd.cls.hbrBackground = NULL;
		wnd_data[0].wnd.cls.lpszMenuName = NULL;
		wnd_data[0].wnd.cls.lpszClassName = CLASS_NAME;
		wnd_data[0].wnd.cls.hIconSm = NULL;
		if (!is_class_registered(wnd_data) && RegisterClassEx(&wnd_data[0].wnd.cls) == INVALID_ATOM) {
			err[0] = error_new(50, GetLastError(), NULL);
			wnd_data[0].wnd.cls.lpszClassName = NULL;
			wnd_data[0].destroy(data, err);
		}
	}
}

static void ogl30_window_create(void *const data, void **const err) {
	if (err[0] == NULL) {
		window_data_t *const wnd_data = (window_data_t*)data;
		LPCTSTR const title = ensure_title(wnd_data);
		const int x = wnd_data[0].client.x_wnd;
		const int y = wnd_data[0].client.y_wnd;
		const int width = wnd_data[0].client.width_wnd;
		const int height = wnd_data[0].client.height_wnd;
		const DWORD style = wnd_data[0].style;
		wnd_data[0].wnd.hndl = CreateWindow(wnd_data[0].wnd.cls.lpszClassName, title, style, x, y, width, height, NULL, NULL, wnd_data[0].wnd.cls.hInstance, (LPVOID)data);
		if (wnd_data[0].wnd.hndl) {
			if (wnd_data[0].title) {
				free(wnd_data[0].title);
				wnd_data[0].title = NULL;
			}
		} else {
			err[0] = error_new(51, GetLastError(), NULL);
			wnd_data[0].destroy(data, err);
		}
	}
}

static void ogl30_context_create(void *const data, void **const err) {
	if (err[0] == NULL) {
		window_data_t *const wnd_data = (window_data_t*)data;
		wnd_data[0].wnd.ctx.dc = GetDC(wnd_data[0].wnd.hndl);
		if (wnd_data[0].wnd.ctx.dc) {
			int pixelFormat;
			BOOL status = FALSE;
			UINT numFormats = 0;
			const int pixelAttribs[] = {
				WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
				WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
				WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
				/* WGL_SWAP_COPY_ARB might have update problems in fullscreen */
				/* WGL_SWAP_EXCHANGE_ARB might have problems with start menu in fullscreen */
				WGL_SWAP_METHOD_ARB, WGL_SWAP_EXCHANGE_ARB,
				WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
				WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
				WGL_COLOR_BITS_ARB, 32,
				WGL_ALPHA_BITS_ARB, 8,
				WGL_DEPTH_BITS_ARB, 24,
				0
			};
			const int contextAttributes[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
				WGL_CONTEXT_MINOR_VERSION_ARB, 0,
				WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
				0
			};
			status = wnd_data[0].wglChoosePixelFormatARB(wnd_data[0].wnd.ctx.dc, pixelAttribs, NULL, 1, &pixelFormat, &numFormats);
			if (status && numFormats) {
				PIXELFORMATDESCRIPTOR pfd;
				ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
				DescribePixelFormat(wnd_data[0].wnd.ctx.dc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
				if (SetPixelFormat(wnd_data[0].wnd.ctx.dc, pixelFormat, &pfd)) {
					wnd_data[0].wnd.ctx.rc = wnd_data[0].wglCreateContextAttribsARB(wnd_data[0].wnd.ctx.dc, 0, contextAttributes);
					if (!wnd_data[0].wnd.ctx.rc) {
						err[0] = error_new(55, GetLastError(), NULL);
						wnd_data[0].destroy(data, err);
					}
				} else {
					err[0] = error_new(54, GetLastError(), NULL);
					wnd_data[0].destroy(data, err);
				}
			} else {
				err[0] = error_new(53, GetLastError(), NULL);
				wnd_data[0].destroy(data, err);
			}
		} else {
			err[0] = error_new(52, ERROR_SUCCESS, NULL);
			wnd_data[0].destroy(data, err);
		}
	}
}

static void ogl30_destroy(void *const data, void **const err) {
	window_data_t *const wnd_data = (window_data_t*)data;
	BOOL quit = FALSE;
	if (wnd_data[0].wnd.ctx.rc) {
		if (!wglDeleteContext(wnd_data[0].wnd.ctx.rc) && err[0] == NULL) {
			err[0] = error_new(58, GetLastError(), NULL);
		}
		wnd_data[0].wnd.ctx.rc = NULL;
	}
	if (wnd_data[0].wnd.ctx.dc) {
		ReleaseDC(wnd_data[0].wnd.hndl, wnd_data[0].wnd.ctx.dc);
		wnd_data[0].wnd.ctx.dc = NULL;
	}
	if (wnd_data[0].wnd.hndl) {
		if (!DestroyWindow(wnd_data[0].wnd.hndl) && err[0] == NULL) {
			err[0] = error_new(59, GetLastError(), NULL);
		}
		wnd_data[0].wnd.hndl = NULL;
	}
	if (wnd_data[0].wnd.cls.lpszClassName) {
		quit = TRUE;
		if (!UnregisterClass(wnd_data[0].wnd.cls.lpszClassName, wnd_data[0].wnd.cls.hInstance) && err[0] == NULL) {
			const DWORD err_win32 = GetLastError();
			if (err_win32 != ERROR_CLASS_HAS_WINDOWS)
				err[0] = error_new(60, err_win32, NULL);
		}
		wnd_data[0].wnd.cls.lpszClassName = NULL;
	}
	if (wnd_data[0].title) {
		free(wnd_data[0].title);
		wnd_data[0].title = NULL;
	}
	/* stop event queue thread */
	if (quit && !is_class_registered(wnd_data))
		PostQuitMessage(0);
}

static void ogl30_free(void *const data, void **const err) {
	window_data_t *const wnd_data = (window_data_t*)data;
	if (wnd_data[0].title)
		free(wnd_data[0].title);
	free(data);
}
