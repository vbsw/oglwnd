/*
 *          Copyright 2022, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

static BOOL ogl30_message_proc(void *const data, HWND const hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam, LRESULT *const result) {
	window_data_t *const wnd_data = (window_data_t*)data;
	switch (message) {
	case WM_CLOSE:
		goOnClose(wnd_data->go_obj_id);
		return TRUE;
	}
	return FALSE;
}


/*
	LRESULT result = 0;
	if (running && !state.minimized) {
		switch (message) {
		case WM_MOVE:
			update_client_props(client.width, client.height);
			result = DefWindowProc(hWnd, message, wParam, lParam);
			goOnMove();
			break;
		case WM_SIZE:
			update_client_props((int)LOWORD(lParam), (int)HIWORD(lParam));
			if (state.dragging) {
				if (!state.maximized)
					maximize_begin();
				else
					maximize_end();
			}
			goOnResize();
			result = DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_SETFOCUS:
			state.focus = 1;
			goOnFocusGain();
			break;
		case WM_KILLFOCUS:
			state.focus = 0;
			clear_keys();
			clear_clip_cursor();
			goOnFocusLoose();
			break;
		case WM_CLOSE:
			goOnClose();
			result = DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_SETCURSOR:
			if (LOWORD(lParam) == HTCLIENT) {
				SetCursor(mouse.cursor);
				result = TRUE;
			} else {
				result = DefWindowProc(hWnd, message, wParam, lParam);
			}
			result = DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_GETMINMAXINFO:
			get_window_min_max((LPMINMAXINFO)lParam);
			break;
		case WM_NCHITTEST:
			result = DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_NCMOUSEMOVE:
			drag_end();
			resize_end();
			break;
		case WM_NCLBUTTONDOWN:
			result = DefWindowProc(window.hndl, WM_NCHITTEST, wParam, lParam);
			if (result == HTCAPTION)
				drag_begin();
			else if (result == HTTOPLEFT || result == HTTOP || result == HTTOPRIGHT || result == HTRIGHT || result == HTBOTTOMRIGHT || result == HTBOTTOM || result == HTBOTTOMLEFT || result == HTLEFT)
				resize_begin();
			result = DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_NCLBUTTONUP:
			result = DefWindowProc(hWnd, message, wParam, lParam);
			drag_end();
			resize_end();
			break;
		case WM_NCLBUTTONDBLCLK:
			drag_end();
			resize_end();
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
				maximize_begin();
			} else if (wParam == SC_RESTORE && state.maximized) {
				state.maximized = 0;
				goOnRestore();
			} else if (wParam == SC_MOVE) {
				drag_begin();
			} else if (wParam == SC_SIZE) {
				resize_begin();
			}
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
			if (config.locked && !state.locked && state.focus)
				update_clip_cursor();
			break;
		case WM_LBUTTONDOWN:
			process_lb_down(message, wParam, lParam, 0);
			break;
		case WM_LBUTTONUP:
			ReleaseCapture();
			if (state.dragging_cust) {
				state.dragging_cust = 0;
				goOnDragCustEnd();
			}
			goOnButtonUp(1);
			break;
		case WM_LBUTTONDBLCLK:
			process_lb_down(message, wParam, lParam, 1);
			break;
		case WM_RBUTTONDOWN:
			goOnButtonDown(2, 0);
			break;
		case WM_RBUTTONUP:
			goOnButtonUp(2);
			break;
		case WM_RBUTTONDBLCLK:
			goOnButtonDown(2, 1);
			break;
		case WM_MBUTTONDOWN:
			goOnButtonDown(3, 0);
			break;
		case WM_MBUTTONUP:
			goOnButtonUp(3);
			break;
		case WM_MBUTTONDBLCLK:
			goOnButtonDown(3, 1);
			break;
		case WM_MOUSEWHEEL:
			goOnWheel((float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA);
			break;
		case WM_XBUTTONDOWN:
			if (HIWORD(wParam) == XBUTTON1)
				goOnButtonDown(4, 0);
			else if (HIWORD(wParam) == XBUTTON2)
				goOnButtonDown(5, 0);
			break;
		case WM_XBUTTONUP:
			if (HIWORD(wParam) == XBUTTON1)
				goOnButtonUp(4);
			else if (HIWORD(wParam) == XBUTTON2)
				goOnButtonUp(5);
			break;
		case WM_XBUTTONDBLCLK:
			if (HIWORD(wParam) == XBUTTON1)
				goOnButtonDown(4, 1);
			else if (HIWORD(wParam) == XBUTTON2)
				goOnButtonDown(5, 1);
			break;
		case WM_ENTERMENULOOP:
			goOnMenuEnter();
			result = DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_EXITMENULOOP:
			result = DefWindowProc(hWnd, message, wParam, lParam);
			goOnMenuLeave();
			drag_end();
			resize_end();
			update_mouse_pos();
			break;
		case WM_EXITSIZEMOVE:
			result = DefWindowProc(hWnd, message, wParam, lParam);
			drag_end();
			resize_end();
			break;
		default:
			result = DefWindowProc(hWnd, message, wParam, lParam);
		}
	} else {
		if (message == WM_DESTROY)
			// stop event queue thread
			PostQuitMessage(0);
		result = DefWindowProc(hWnd, message, wParam, lParam);
		if (message == WM_SETFOCUS) {
			state.focus = 1;
			// restore from minimized and avoid move/resize events
			if (state.minimized) {
				state.minimized = 0;
				goOnRestore();
				drag_end();
				resize_end();
			}
		}
	}
	return result;
*/
