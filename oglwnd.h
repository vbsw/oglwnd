#ifndef OGLWND_H
#define OGLWND_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(OGLWND_WIN32)
typedef unsigned long oglwnd_ul_t;
extern void oglwnd_error(void *err, int *err_num, oglwnd_ul_t *err_win32, char **err_str);
extern void oglwnd_free(void *data);
extern void oglwnd_window_allocate(void **data, void **err);
extern void oglwnd_window_free(void *data, void **err);
extern void oglwnd_window_init_dummy(void *data, void **err);
extern void oglwnd_window_init_opengl30(void *data, int go_obj, int x, int y, int w, int h, int wn, int hn, int wx, int hx, int b, int d, int r, int f, int l, int c, void **err);
extern void oglwnd_window_create(void *data, void **err);
extern void oglwnd_window_destroy(void *data, void **err);
extern void oglwnd_context_make_current(void *data, void **err);
extern void oglwnd_context_release(void *data, void **err);
extern void oglwnd_context_swap_buffers(void *data, void **err);
extern void oglwnd_window_context(void *data, void **ctx);
extern void oglwnd_window_show(void *data, void **err);
extern int oglwnd_window_funcs_avail(void *data);
extern int oglwnd_window_dt_func_avail(void *data);
extern void oglwnd_process_events();
#elif defined(OGLWND_LINUX)
#endif

#ifdef __cplusplus
}
#endif

#endif /* OGLWND_H */