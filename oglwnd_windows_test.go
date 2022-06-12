/*
 *          Copyright 2022, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

package oglwnd

import (
	"testing"
)

func TestAllocate(t *testing.T) {
	var window Window
	err := window.allocate()
	if err == nil {
		if window.Ptr != nil {
			err = window.ReleaseMemory()
			if err == nil {
				if window.Ptr != nil {
					t.Error("window pointer not cleared")
				}
			} else {
				t.Error(err.Error())
			}
		} else {
			t.Error("window pointer not set")
		}
	} else {
		t.Error(err.Error())
	}
}

func TestDummy(t *testing.T) {
	var params Parameters
	dummy := New()
	params.Dummy = true
	err := dummy.Init(&params)
	if err == nil {
		err = dummy.Create()
		if err == nil {
			err = dummy.Destroy()
			if err == nil {
				err = dummy.ReleaseMemory()
				if err != nil {
					t.Error(err.Error())
				}
			} else {
				t.Error(err.Error())
			}
		} else {
			t.Error(err.Error())
		}
	} else {
		t.Error(err.Error())
	}
}

func TestContextDummy(t *testing.T) {
	var params Parameters
	dummy := New()
	params.Dummy = true
	err := dummy.Init(&params)
	if err == nil {
		err = dummy.Create()
	}
	if err == nil {
		ctx := dummy.Context()
		if ctx != nil {
			err = ctx.MakeCurrent()
			if err == nil {
				err = ctx.SwapBuffers()
				if err == nil {
					err = ctx.Release()
					if err != nil {
						t.Error(err.Error())
					}
				} else {
					t.Error(err.Error())
				}
			} else {
				t.Error(err.Error())
			}
			err = nil
		} else {
			t.Error("no dummy context")
		}
		err = dummy.Destroy()
	}
	if err == nil {
		err = dummy.ReleaseMemory()
	}
	if err != nil {
		t.Error(err.Error())
	}
}

/*
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
*/