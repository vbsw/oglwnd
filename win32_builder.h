#ifndef WIN32_BUILDER_H
#define WIN32_BUILDER_H

typedef void(*init_module_f) (HINSTANCE *instance, int *err_num, char **err_str_extra);
typedef void(*new_dummy_f) (dummy_data_t **data, HINSTANCE instance, int *err_num, char **err_str_extra);
typedef void(*init_dummy_class_f) (dummy_data_t *data, HINSTANCE instance, int *err_num, char **err_str_extra);
typedef void(*init_dummy_window_f) (dummy_data_t *data, int *err_num, char **err_str_extra);
typedef void(*init_dummy_context_f) (dummy_data_t *data, int *err_num, char **err_str_extra);
typedef void(*destroy_dummy_f) (dummy_data_t *data);
typedef void(*init_wgl_f) (dummy_data_t *data, void *cpfarb, void *ccaarb, int *err_num, char **err_str_extra);

typedef struct {
	init_module_f        init_module;
	new_dummy_f          new_dummy;
	init_dummy_class_f   init_dummy_class;
	init_dummy_window_f  init_dummy_window;
	init_dummy_context_f init_dummy_context;
	destroy_dummy_f      destroy_dummy;
	init_wgl_f           init_wgl;
	new_window_f         new_window;
	init_class_f         init_class;
	init_window_f        init_window;
	init_context_f       init_context;
} builder_t;

static void init_module(HINSTANCE *const instance, int *const err_num, char **const err_str_extra) {
	if (err_num[0] == 0) {
		instance[0] = GetModuleHandle(NULL);
		if (!instance[0]) {
			err_num[0] = 1;
		}
	}
}

static void init_wgl(dummy_data_t *const data, void *const cpfarb, void *const ccaarb, int *const err_num, char **const err_str_extra) {
	if (err_num[0] == 0) {
		if (wglMakeCurrent(data->window.dc, data->window.rc)) {
			PFNWGLCHOOSEPIXELFORMATARBPROC *const wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC*)cpfarb;
			wglChoosePixelFormatARB[0] = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
			if (wglChoosePixelFormatARB[0]) {
				PFNWGLCREATECONTEXTATTRIBSARBPROC *const wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC*)ccaarb;
				wglCreateContextAttribsARB[0] = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
				if (!wglCreateContextAttribsARB[0]) {
					err_num[0] = 10;
				}
			} else {
				err_num[0] = 9;
			}
		} else {
			err_num[0] = 8;
		}
	}
}

void oglwnd_new_builder(void **const builder) {
	builder_t *const bldr = (builder_t*)malloc(sizeof(builder_t));
	bldr->init_module = init_module;
	bldr->new_dummy = new_dummy_impl;
	bldr->init_dummy_class = init_dummy_class_impl;
	bldr->init_dummy_window = init_dummy_window_impl;
	bldr->init_dummy_context = init_dummy_context_impl;
	bldr->destroy_dummy = destroy_dummy_impl;
	bldr->init_wgl = init_wgl;
	bldr->new_window = new_window_impl;
	bldr->init_class = init_class_impl;
	bldr->init_window = init_window_impl;
	bldr->init_context = init_context_impl;
	builder[0] = bldr;
}

void oglwnd_destroy_builder(void *const builder) {
	free(builder);
}

#endif /* WIN32_BUILDER_H */
