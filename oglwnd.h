#ifndef OGLWND_H
#define OGLWND_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_OGLWND_WIN32)
extern void oglwnd_init(int x, int y, int w, int h, int wMin, int hMin, int wMax, int hMax, int c, int b, int d, int r, int f, int l, int *err);
extern void oglwnd_clear(float r, float g, float b, float a);
extern void oglwnd_get_window_props(int *x, int *y, int *w, int *h, int *wMin, int *hMin, int *wMax, int *hMax, int *b, int *d, int *r, int *f, int *l);
extern void oglwnd_set_window_props(int x, int y, int w, int h, int wMin, int hMin, int wMax, int hMax, int b, int d, int r, int f, int l);
extern void oglwnd_show();
extern void oglwnd_main_loop();
extern void oglwnd_destroy();
extern void oglwnd_stop();
#elif defined(_OGLWND_LINUX)
#endif

#ifdef __cplusplus
}
#endif

#endif /* OGLWND_H */