#ifndef WIN32_DUMMY_H
#define WIN32_DUMMY_H

#define DUMMY_CLASS_NAME TEXT("oglwnd_cls_dmy")

static void new_dummy_impl(dummy_data_t **const data, HINSTANCE const instance, int *const err_num, char **const err_str_extra) {
	data[0] = (dummy_data_t*)malloc(sizeof(dummy_data_t));
	ZeroMemory(data[0], sizeof(dummy_data_t));
}

static void init_dummy_class_impl(dummy_data_t *const data, HINSTANCE const instance, int *const err_num, char **const err_str_extra) {
	if (err_num[0] == 0) {
		data->window.cls.cbSize = sizeof(WNDCLASSEX);
		data->window.cls.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		data->window.cls.lpfnWndProc = DefWindowProc;
		data->window.cls.cbClsExtra = 0;
		data->window.cls.cbWndExtra = 0;
		data->window.cls.hInstance = instance;
		data->window.cls.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		data->window.cls.hCursor = cursor.arrow;
		data->window.cls.hbrBackground = NULL;
		data->window.cls.lpszMenuName = NULL;
		data->window.cls.lpszClassName = DUMMY_CLASS_NAME;
		data->window.cls.hIconSm = NULL;
		if (RegisterClassEx(&data->window.cls) == INVALID_ATOM)
			err_num[0] = 2;
	}
}

static void init_dummy_window_impl(dummy_data_t *const data, int *const err_num, char **const err_str_extra) {
	if (err_num[0] == 0) {
		data->window.hndl = CreateWindow(DUMMY_CLASS_NAME, TEXT("Dummy"), WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, NULL, NULL, data->window.cls.hInstance, NULL);
		if (!data->window.hndl) {
			err_num[0] = 3;
		}
	}
}

static void init_dummy_context_impl(dummy_data_t *const data, int *const err_num, char **const err_str_extra) {
	if (err_num[0] == 0) {
		data->window.dc = GetDC(window.hndl);
		if (data->window.dc) {
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
			pixelFormat = ChoosePixelFormat(data->window.dc, &pixelFormatDesc);
			if (pixelFormat) {
				if (SetPixelFormat(data->window.dc, pixelFormat, &pixelFormatDesc)) {
					data->window.rc = wglCreateContext(data->window.dc);
					if (!data->window.rc) {
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

static void destroy_dummy_impl(dummy_data_t *const data) {
	if (data) {
		if (data->window.rc) {
			wglMakeCurrent(data->window.dc, NULL);
			wglDeleteContext(data->window.rc);
		}
		if (data->window.dc) {
			ReleaseDC(data->window.hndl, data->window.dc);
		}
		if (data->window.hndl) {
			DestroyWindow(data->window.hndl);
		}
		UnregisterClass(data->window.cls.lpszClassName, data->window.cls.hInstance);
		free(data);
	}
}

#endif /* WIN32_DUMMY_H */
