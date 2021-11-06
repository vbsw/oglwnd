#ifndef WIN32_DUMMY_H
#define WIN32_DUMMY_H

#define DUMMY_CLASS_NAME TEXT("oglwnd_cls_dmy")

static void new_dummy_impl(window_t **const dummy, HINSTANCE const instance, int *const err_num, char **const err_str_extra) {
	dummy[0] = (window_t*)malloc(sizeof(window_t));
	ZeroMemory(dummy[0], sizeof(window_t));
}

static void init_dummy_class_impl(window_t *const dummy, HINSTANCE const instance, int *const err_num, char **const err_str_extra) {
	if (err_num[0] == 0) {
		dummy->cls.cbSize = sizeof(WNDCLASSEX);
		dummy->cls.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		dummy->cls.lpfnWndProc = DefWindowProc;
		dummy->cls.cbClsExtra = 0;
		dummy->cls.cbWndExtra = 0;
		dummy->cls.hInstance = instance;
		dummy->cls.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		dummy->cls.hCursor = cursor.arrow;
		dummy->cls.hbrBackground = NULL;
		dummy->cls.lpszMenuName = NULL;
		dummy->cls.lpszClassName = DUMMY_CLASS_NAME;
		dummy->cls.hIconSm = NULL;
		if (RegisterClassEx(&dummy->cls) == INVALID_ATOM)
			err_num[0] = 2;
	}
}

static void init_dummy_window_impl(window_t *const dummy, int *const err_num, char **const err_str_extra) {
	if (err_num[0] == 0) {
		dummy->hndl = CreateWindow(DUMMY_CLASS_NAME, TEXT("Dummy"), WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, NULL, NULL, dummy->cls.hInstance, NULL);
		if (!dummy->hndl) {
			err_num[0] = 3;
		}
	}
}

static void init_dummy_context_impl(window_t *const dummy, int *const err_num, char **const err_str_extra) {
	if (err_num[0] == 0) {
		dummy->dc = GetDC(window.hndl);
		if (dummy->dc) {
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
			pixelFormat = ChoosePixelFormat(dummy->dc, &pixelFormatDesc);
			if (pixelFormat) {
				if (SetPixelFormat(dummy->dc, pixelFormat, &pixelFormatDesc)) {
					dummy->rc = wglCreateContext(dummy->dc);
					if (!dummy->rc) {
						err_num[0] = 7;
					}
				} else {
					err_num[0] = 6;
				}
			} else {
				err_num[0] = 5;
			}
		} else {
			err_num[0] = 4;
		}
	}
}

static void destroy_dummy_impl(window_t *const dummy) {
	if (dummy) {
		if (dummy->rc) {
			wglMakeCurrent(dummy->dc, NULL);
			wglDeleteContext(dummy->rc);
		}
		if (dummy->dc) {
			ReleaseDC(dummy->hndl, dummy->dc);
		}
		if (dummy->hndl) {
			DestroyWindow(dummy->hndl);
		}
		if (dummy->cls.lpszClassName)
			UnregisterClass(dummy->cls.lpszClassName, instance);
		free(dummy);
	}
}

#endif /* WIN32_DUMMY_H */
