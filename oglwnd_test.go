/*
 *          Copyright 2021, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

package oglwnd

import (
	"testing"
)

func TestInitOpenGL(t *testing.T) {
	hn = new(DefaultHandler)
	initOGLWindow()
	if err == nil {
		destroyWindow()
	} else {
		t.Error(err.Error())
	}
}
