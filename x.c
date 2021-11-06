/*
 *          Copyright 2021, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

#if defined(OGLWND_X)

#include <stdlib.h>

void oglwnd_new_builder(void **const builder) {
	// TODO
}

void oglwnd_destroy_builder(void *const builder, char *const str) {
	// TODO
	if (str)
		free(str);
}

void oglwnd_init(void *const builder, int *const err_num, char **const err_str_extra) {
	// TODO
}

void oglwnd_destroy() {
	// TODO
}

/* #if defined(_OGLWIN_WIN32) */
#endif
