/*
 *          Copyright 2021, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

#if defined(_OGLWND_WIN32)

#include "win32.h"
#include "win32_dummy.h"
#include "win32_builder.h"

void oglwnd_init(void *const builder, int *const err_num, char **const err_str_extra) {
	if (err_num[0] == 0) {
		window_t *dummy = NULL;
		builder_t *const bldr = (builder_t*)builder;
		bldr->init_module(&instance, err_num, err_str_extra);
		bldr->new_dummy(&dummy, instance, err_num, err_str_extra);
		bldr->destroy_dummy(dummy);
	}
}

void oglwnd_destroy() {
}

/* #if defined(_OGLWIN_WIN32) */
#endif
