/*
 *          Copyright 2021, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

package oglwnd

// #cgo CFLAGS: -DOGLWND_LINUX
// #include "oglwnd.h"
import "C"

// Init initializes functions needed to create windows with OpenGL context.
func Init() error {
	if !initialized {
		return nil
	}
	return nil
}
