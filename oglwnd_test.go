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

func TestInit(t *testing.T) {
	err := Init()
	if err == nil {
		window, errWnd := New(nil)
		if errWnd == nil {
			window.Quit()
		} else {
			t.Error(err.Error())
		}
	} else {
		t.Error(err.Error())
	}
}

func TestUint64ToString(t *testing.T) {
	strA := uint64ToString(0)
	strB := uint64ToString(9)
	strC := uint64ToString(1000)
	strD := uint64ToString(10000000000)
	strE := uint64ToString(11123456781187654311)
	strF := uint64ToString(11999999999999999999)
	if strA != "0" {
		t.Error(strA)
	} else if strB != "9" {
		t.Error(strB)
	} else if strC != "1000" {
		t.Error(strC)
	} else if strD != "10000000000" {
		t.Error(strD)
	} else if strE != "11123456781187654311" {
		t.Error(strE)
	} else if strF != "11999999999999999999" {
		t.Error(strF)
	}
}
