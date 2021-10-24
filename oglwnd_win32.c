/*
 *          Copyright 2021, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

#if defined(_OGLWND_WIN32)

#include "oglwnd_win32.h"

//MessageBox(NULL, TEXT("hallo"), TEXT("yoyo"), MB_OK | MB_ICONEXCLAMATION);

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

static void config_wnd_pos_size(const DWORD style) {
	RECT rect = { 0, 0, config.width, config.height };
	AdjustWindowRect(&rect, style, FALSE);
	config.wnd_width = rect.right - rect.left;
	config.wnd_height = rect.bottom - rect.top;

	if (config.centered) {
		config.wnd_x = monitor.x + (monitor.width - config.wnd_width) / 2;
		config.wnd_y = monitor.y + (monitor.height - config.wnd_height) / 2;
	} else {
		config.wnd_x = config.x;
		config.wnd_y = config.y;
	}
}

static void get_window_props(const DWORD style, const int clientX, const int clientY, const int clientW, const int clientH, int *const x, int *const y, int *const w, int *const h) {
	RECT rect = { clientX, clientY, clientX + clientW, clientY + clientH };
	AdjustWindowRect(&rect, style, FALSE);
	*x = rect.left;
	*y = rect.top;
	*w = rect.right - rect.left;
	*h = rect.bottom - rect.top;
}

static void set_window_pos(const int clientX, const int clientY, const int clientW, const int clientH) {
	const DWORD style = get_style();
	int x, y, w, h;
	get_window_props(style, clientX, clientY, clientW, clientH, &x, &y, &w, &h);
	SetWindowLong(window.hndl, GWL_STYLE, style);
	SetWindowPos(window.hndl, HWND_TOP, x, y, w, h, SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
}

static void move_window(const int clientX, const int clientY, const int clientW, const int clientH) {
	const DWORD style = get_style();
	int x, y, w, h, s;
	get_window_props(style, clientX, clientY, clientW, clientH, &x, &y, &w, &h);
	MoveWindow(window.hndl, x, y, w, h, FALSE);
}

static void backup_client_props() {
	client.xW = client.x;
	client.yW = client.y;
	client.widthW = client.width;
	client.heightW = client.height;
}

static void restore_client_props() {
	client.x = client.xW;
	client.y = client.yW;
	client.width = client.widthW;
	client.height = client.heightW;
}

static void update_client_props(const int width, const int height) {
	POINT point = { 0, 0 };
	ClientToScreen(window.hndl, &point);
	client.x = point.x;
	client.y = point.y;
	client.width = width;
	client.height = height;
}

static void update_mouse_pos() {
	POINT point;
	GetCursorPos(&point);
	goDebug4((int)point.x, (int)point.y, (int)client.x, (int)client.y);
	mouse.x = point.x - client.x;
	mouse.y = point.y - client.y;
}

static void set_fullscreen() {
	backup_client_props();
	SetWindowLong(window.hndl, GWL_STYLE, 0);
	SetWindowPos(window.hndl, HWND_TOP, monitor.x, monitor.y, monitor.width, monitor.height, SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
}

static int process_key_down(const UINT message, const WPARAM wParam, const LPARAM lParam) {
	const int code = get_keycode(message, wParam, lParam);
	if (code) {
		goOnKeyDown(code, pressed[code]++);
		return 1;
	}
	return 0;
}

static int process_key_up(const UINT message, const WPARAM wParam, const LPARAM lParam) {
	const int code = get_keycode(message, wParam, lParam);
	if (code) {
		pressed[code] = 0;
		goOnKeyUp(code);
		return 1;
	}
	return 0;
}

static int process_lb_down(const UINT message, const WPARAM wParam, const LPARAM lParam, const int double_click) {
	int processed = 0;
	const LRESULT result = DefWindowProc(window.hndl, WM_NCHITTEST, wParam, lParam);
	if (result == HTNOWHERE || result == HTCLIENT) {
		/* hit client */
		if (mouse.x >= 0 && mouse.x <= client.width && mouse.y >= 0 && mouse.y <= client.height) {
			if (config.dragable && !state.maximized) {
				state.dragging_cust = 1;
				goOnDragCustBegin();
			}
			SetCapture(window.hndl);
			processed = 1;
		}
	}
	return processed;
}

static void drag_begin() {
	if (!state.dragging) {
		state.dragging = 1;
		goOnDragBegin();
	}
}

static void drag_end() {
	if (state.dragging) {
		state.dragging = 0;
		goOnDragEnd();
	}
}

static void maximize_begin() {
	if (!state.maximized) {
		state.maximized = 1;
		goOnMaximize();
	}
}

static void maximize_end() {
	if (state.maximized) {
		state.maximized = 0;
		goOnRestore();
	}
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
		if (!dummy.hndl) {
			*err = 3;
		}
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

static void init_monitor(int *const err) {
	if (*err == 0) {
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

static void get_window_min_max(LPMINMAXINFO const lpMMI) {
	const int sysWidthMin = GetSystemMetrics(SM_CXMINTRACK);
	const int sysHeightMin = GetSystemMetrics(SM_CYMINTRACK);
	const int sysWidthMax = GetSystemMetrics(SM_CXMAXTRACK);
	const int sysHeightMax = GetSystemMetrics(SM_CYMAXTRACK);

	if (config.fullscreen) {
		lpMMI->ptMinTrackSize.x = sysWidthMin;
		lpMMI->ptMinTrackSize.y = sysHeightMin;
		lpMMI->ptMaxTrackSize.x = sysWidthMax;
		lpMMI->ptMaxTrackSize.y = sysHeightMax;

	} else {
		const DWORD style = get_style();
		RECT rectMin = { 0, 0, config.widthMin, config.heightMin };
		RECT rectMax = { 0, 0, config.widthMax, config.heightMax };

		AdjustWindowRect(&rectMin, style, FALSE);
		AdjustWindowRect(&rectMax, style, FALSE);

		const int wndWidthMin = rectMin.right - rectMin.left;
		const int wndHeightMin = rectMin.bottom - rectMin.top;
		const int wndWidthMax = rectMax.right - rectMax.left;
		const int wndHeightMax = rectMax.bottom - rectMax.top;

		if (sysWidthMin <= wndWidthMin && wndWidthMin < wndWidthMax && wndWidthMin < sysWidthMax)
			lpMMI->ptMinTrackSize.x = wndWidthMin;
		else
			lpMMI->ptMinTrackSize.x = sysWidthMin;
		if (sysHeightMin <= wndHeightMin && wndHeightMin < wndHeightMax && wndHeightMin < sysHeightMax)
			lpMMI->ptMinTrackSize.y = wndHeightMin;
		else
			lpMMI->ptMinTrackSize.y = sysHeightMin;
		if (sysWidthMax >= wndWidthMax && wndWidthMax > wndWidthMin && wndWidthMax > sysWidthMin)
			lpMMI->ptMaxTrackSize.x = wndWidthMax;
		else
			lpMMI->ptMaxTrackSize.x = sysWidthMax;
		if (sysHeightMax >= wndHeightMax && wndHeightMax > wndHeightMin && wndHeightMax > sysHeightMin)
			lpMMI->ptMaxTrackSize.y = wndHeightMax;
		else
			lpMMI->ptMaxTrackSize.y = sysHeightMax;
	}
}

static LRESULT CALLBACK windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
	if (running && !state.minimized) {
		switch (message) {
		case WM_MOVE:
			update_client_props(client.width, client.height);
			result = DefWindowProc(hWnd, message, wParam, lParam);
			goOnWindowMove();
			break;
		case WM_SIZE:
			update_client_props((int)LOWORD(lParam), (int)HIWORD(lParam));
			if (state.dragging) {
				if (!state.maximized)
					maximize_begin();
				else
					maximize_end();
			}
			goOnWindowSize();
			result = DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_CLOSE:
			result = DefWindowProc(hWnd, message, wParam, lParam);
			goOnClose();
			break;
		case WM_GETMINMAXINFO:
			get_window_min_max((LPMINMAXINFO)lParam);
			break;
		case WM_NCHITTEST:
			result = DefWindowProc(hWnd, message, wParam, lParam);
			if (result == HTCAPTION)
				result = HTCLIENT;
			break;
		case WM_NCMOUSEMOVE:
			drag_end();
			break;
		case WM_NCLBUTTONDOWN:
			if (DefWindowProc(window.hndl, WM_NCHITTEST, wParam, lParam) == HTCAPTION)
				drag_begin();
			result = DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_NCLBUTTONUP:
			result = DefWindowProc(hWnd, message, wParam, lParam);
			drag_end();
			break;
		case WM_NCLBUTTONDBLCLK:
			drag_end();
			result = DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_KEYDOWN:
			if (!process_key_down(message, wParam, lParam))
				result = DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_KEYUP:
			if (!process_key_up(message, wParam, lParam))
				result = DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_SYSKEYDOWN:
			if (!process_key_down(message, wParam, lParam))
				result = DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_SYSKEYUP:
			if (!process_key_up(message, wParam, lParam))
				result = DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_SYSCOMMAND:
			if (wParam == SC_MINIMIZE) {
				state.minimized = 1;
				goOnMinimize();
			} else if (wParam == SC_MAXIMIZE) {
				state.maximized = 1;
				goOnMaximize();
			} else if (wParam == SC_RESTORE && state.maximized) {
				state.maximized = 0;
				goOnRestore();
			}
			/* ignore window move/resize from menu */
			if (wParam != SC_MOVE && wParam != SC_SIZE)
				result = DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_MOUSEMOVE:
			if (state.dragging_cust && !state.maximized) {
				move_window(client.x + (int)(short)LOWORD(lParam) - mouse.x, client.y + (int)(short)HIWORD(lParam) - mouse.y, client.width, client.height);
			} else {
				mouse.x = ((int)(short)LOWORD(lParam));
				mouse.y = ((int)(short)HIWORD(lParam));
				result = DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_LBUTTONDOWN:
			if (!process_lb_down(message, wParam, lParam, 0))
				result = DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_LBUTTONUP:
			ReleaseCapture();
			if (state.dragging_cust) {
				state.dragging_cust = 0;
				goOnDragCustEnd();
			}
			break;
		case WM_LBUTTONDBLCLK:
			if (!process_lb_down(message, wParam, lParam, 1))
				result = DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_ENTERMENULOOP:
			goOnMenuEnter();
			result = DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_EXITMENULOOP:
			result = DefWindowProc(hWnd, message, wParam, lParam);
			goOnMenuLeave();
			update_mouse_pos();
			break;
		case WM_EXITSIZEMOVE:
			result = DefWindowProc(hWnd, message, wParam, lParam);
			drag_end();
			break;
		default:
			result = DefWindowProc(hWnd, message, wParam, lParam);
		}
	} else {
		if (message == WM_DESTROY)
			/* stop event queue thread */
			PostQuitMessage(0);
		result = DefWindowProc(hWnd, message, wParam, lParam);
		/* restore from minimized and avoid move/resize events */
		if (message == WM_SETFOCUS && state.minimized) {
			state.minimized = 0;
			goOnRestore();
		}
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
		const DWORD style = get_style();
		config_wnd_pos_size(style);
		window.hndl = CreateWindow(CLASS_NAME, TEXT("OpenGL"), style, config.wnd_x, config.wnd_y, config.wnd_width, config.wnd_height, NULL, NULL, instance, NULL);
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
				/* WGL_SWAP_COPY_ARB has update problems in fullscreen (AMD) */
				/* WGL_SWAP_EXCHANGE_ARB has problems with start menu in fullscreen (AMD) */
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

static void reset_memory() {
	ZeroMemory(pressed, 255 * sizeof(int));
	state.dragging = 0;
	state.dragging_cust = 0;
	state.locked = 0;
	state.minimized = 0;
	state.maximized = 0;
}

void oglwnd_init(int x, int y, int w, int h, int wMin, int hMin, int wMax, int hMax, int c, int b, int d, int r, int f, int *err) {
	int error = 0;
	init_module_handle(&error);
	init_dummy_class(&error);
	init_dummy_window(&error);
	init_dummy_context(&error);
	init_wgl_functions(&error);
	init_monitor(&error);
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
		if (!error) {
			reset_memory();
		} else {
			destroy_window(&window);
		}
	}
	*err = error;
}

void oglwnd_get_window_props(int *x, int *y, int *w, int *h, int *wMin, int *hMin, int *wMax, int *hMax, int *b, int *d, int *r, int *f) {
	*x = client.x;
	*y = client.y;
	*w = client.width;
	*h = client.height;
	*wMin = config.widthMin;
	*hMin = config.heightMin;
	*wMax = config.widthMax;
	*hMax = config.heightMax;
	*b = config.borderless;
	*d = config.dragable;
	*r = config.resizable;
	*f = config.fullscreen;
}

void oglwnd_set_window_props(int x, int y, int w, int h, int wMin, int hMin, int wMax, int hMax, int b, int d, int r, int f) {
	const int xywh = (x != client.x || y != client.y || w != client.width || h != client.height);
	const int mm = (wMin != config.widthMin || hMin != config.heightMin || wMax != config.widthMax || hMax != config.heightMax);
	const int stl = (b != config.borderless || r != config.resizable);
	const int fs = (f != config.fullscreen);
	client.x = x;
	client.y = y;
	client.width = w;
	client.height = h;
	config.widthMin = wMin;
	config.heightMin = hMin;
	config.widthMax = wMax;
	config.heightMax = hMax;
	config.borderless = b;
	config.dragable = d;
	config.resizable = r;
	config.fullscreen = f;
	// fullscreen
	if (fs && f) {
		set_fullscreen();
	// window
	} else if (fs) {
		if (!xywh)
			restore_client_props();
		set_window_pos(client.x, client.y, client.width, client.height);
	} else if (!f) {
		if (stl) {
			set_window_pos(x, y, w, h);
		} else if (xywh) {
			move_window(x, y, w, h);
		}
	}
}

void oglwnd_show() {
	RECT rect;
	ShowWindow(window.hndl, SW_SHOWDEFAULT);
	GetClientRect(window.hndl, &rect);
	update_client_props((int)(rect.right - rect.left), (int)(rect.bottom - rect.top));
	if (config.fullscreen)
		set_fullscreen();
	GetClientRect(window.hndl, &rect);
	update_client_props((int)(rect.right - rect.left), (int)(rect.bottom - rect.top));
	running = TRUE;
	goOnFirstWindowSize();
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
			if (running) {
				goOnUpdate();
				if (running) {
					glFinish();
					SwapBuffers(window.dc);
				}
			}
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
