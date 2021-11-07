#ifndef OGLWND_H
#define OGLWND_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(OGLWND_WIN32)
extern void oglwnd_new_builder(void **builder);
extern void oglwnd_destroy_builder(void *builder);
extern void oglwnd_init(void *builder, int *err_num, char **err_str_extra);
extern void oglwnd_new_window(void *builder, void **data, void *go_data, int *err_num, char **err_str_extra);
extern void oglwnd_destroy_window(void *data);
extern void oglwnd_destroy();
extern void oglwnd_free_mem(void *mem);
extern void oglwnd_process_events();
extern void oglwnd_process_events_blocking();
extern void oglwnd_process_window_events(void *data);
extern void oglwnd_process_window_events_blocking(void *data);
extern void oglwnd_show(void *data);
#elif defined(OGLWND_X)
extern void oglwnd_new_builder(void **builder);
extern void oglwnd_destroy_builder(void *builder, char *str);
extern void oglwnd_init(void *builder, int *err_num, char **err_str_extra);
extern void oglwnd_destroy();
#endif

#ifdef __cplusplus
}
#endif

#endif /* OGLWND_H */