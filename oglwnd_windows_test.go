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

func TestOGL30(t *testing.T) {
	wnd := New()
	err := wnd.Init(nil)
	if err == nil {
		err = wnd.Create()
		if err == nil {
			t.Error("uninitialized wgl functions not recognized")
		}
	} else {
		t.Error(err.Error())
	}
	err = wnd.ReleaseMemory()
	if err != nil {
		t.Error(err.Error())
	}
}
