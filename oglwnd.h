#ifndef OGLWND_H
#define OGLWND_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(OGLWND_WIN32)
typedef unsigned long oglwnd_ul_t;
extern void oglwnd_init(int *err, oglwnd_ul_t *err_win32);
extern void oglwnd_process_events();
extern void oglwnd_process_events_blocking();
extern void oglwnd_window_new(void **data, void *go_obj, int x, int y, int w, int h, int wn, int hn, int wx, int hx, int b, int d, int r, int f, int l, int c, int *err, oglwnd_ul_t *err_win32, char **err_str);
extern void oglwnd_window_init(void *data, int *err, oglwnd_ul_t *err_win32, char **err_str);
extern void oglwnd_ctx_make_current(void *data, int *err, oglwnd_ul_t *err_win32, char **err_str);
extern void oglwnd_ctx_release(void *data, int *err, oglwnd_ul_t *err_win32, char **err_str);
extern void oglwnd_swap_buffers(void *data, int *err, oglwnd_ul_t *err_win32, char **err_str);
extern void oglwnd_window_destroy(void *const data);
extern void oglwnd_free_mem(void *mem);
#elif defined(OGLWND_LINUX)
#endif

#ifdef __cplusplus
}
#endif

#endif /* OGLWND_H */