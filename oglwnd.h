#ifndef OGLWND_H
#define OGLWND_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_OGLWND_WIN32)
extern void oglwnd_new_builder(void **builder);
extern void oglwnd_destroy_builder(void *builder, char *str);
extern void oglwnd_init(void *builder, int *err_num, char **err_str_extra);
extern void oglwnd_destroy();
#elif defined(_OGLWND_X)
extern void oglwnd_new_builder(void **builder);
extern void oglwnd_destroy_builder(void *builder, char *str);
extern void oglwnd_init(void *builder, int *err_num, char **err_str_extra);
extern void oglwnd_destroy();
#endif

#ifdef __cplusplus
}
#endif

#endif /* OGLWND_H */