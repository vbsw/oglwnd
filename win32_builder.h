#ifndef WIN32_BUILDER_H
#define WIN32_BUILDER_H

typedef void(*init_module_f) (HINSTANCE *instance, int *err_num, char **err_str_extra);
typedef void(*new_dummy_f) (window_t **dummy, HINSTANCE instance, int *err_num, char **err_str_extra);
typedef void(*destroy_dummy_f) (window_t *dummy);
typedef void(*init_wgl_f) (window_t *dummy, void *cpfarb, void *ccaarb, int *err_num, char **err_str_extra);

typedef struct {
	init_module_f init_module;
	new_dummy_f new_dummy;
	destroy_dummy_f destroy_dummy;
	init_wgl_f init_wgl;
} builder_t;

static void init_module(HINSTANCE *const instance, int *const err_num, char **const err_str_extra) {
	if (err_num[0] == 0) {
		instance[0] = GetModuleHandle(NULL);
		if (!instance[0]) {
			err_num[0] = 1;
		}
	}
}

static void init_wgl(window_t *const dummy, void *const cpfarb, void *const ccaarb, int *const err_num, char **const err_str_extra) {
	if (err_num[0] == 0) {
		if (wglMakeCurrent(dummy->dc, dummy->rc)) {
			PFNWGLCHOOSEPIXELFORMATARBPROC *const wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC*)cpfarb;
			wglChoosePixelFormatARB[0] = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
			if (wglChoosePixelFormatARB[0]) {
				PFNWGLCREATECONTEXTATTRIBSARBPROC *const wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC*)cpfarb;
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
	bldr->new_dummy = new_dummy;
	bldr->destroy_dummy = destroy_dummy;
	bldr->init_wgl = init_wgl;
	builder[0] = bldr;
}

void oglwnd_destroy_builder(void *const builder, char *const str) {
	free(builder);
	if (str)
		free(str);
}

#endif /* WIN32_BUILDER_H */
