/*
 *          Copyright 2021, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

package oglwnd

// #cgo CFLAGS: -DOGLWND_WIN32
// #cgo LDFLAGS: -luser32 -lgdi32 -lOpenGL32
// #include "oglwnd.h"
import "C"
import (
	"errors"
	"unsafe"
)

// Window is a window with OpenGL context.
type Window struct {
	data                unsafe.Pointer
	handler             Handler
	threaded, updatable bool
}

// tContext provides OpenGL context functions.
type tContext struct {
	data unsafe.Pointer
}

// MakeCurrent makes OpenGL context current to this thread.
func (ctx *tContext) MakeCurrent() error {
	var errC C.int
	var errWin32C C.oglwnd_ul_t
	var errStrC *C.char
	C.oglwnd_ctx_make_current(ctx.data, &errC, &errWin32C, &errStrC)
	if errC == 0 {
		return nil
	}
	err := errNumToError(int(errC), uint64(errWin32C), errStrC)
	if errStrC != nil {
		C.oglwnd_free_mem(unsafe.Pointer(errStrC))
	}
	return err
}

// Release makes all current OpenGL contexts (to this thread) not current.
func (ctx *tContext) Release() error {
	var errC C.int
	var errWin32C C.oglwnd_ul_t
	var errStrC *C.char
	C.oglwnd_ctx_release(ctx.data, &errC, &errWin32C, &errStrC)
	if errC == 0 {
		return nil
	}
	err := errNumToError(int(errC), uint64(errWin32C), errStrC)
	if errStrC != nil {
		C.oglwnd_free_mem(unsafe.Pointer(errStrC))
	}
	return err
}

// SwapBuffers swaps the front and back buffers of the window.
func (ctx *tContext) SwapBuffers() error {
	var errC C.int
	var errWin32C C.oglwnd_ul_t
	var errStrC *C.char
	C.oglwnd_swap_buffers(ctx.data, &errC, &errWin32C, &errStrC)
	if errC == 0 {
		return nil
	}
	err := errNumToError(int(errC), uint64(errWin32C), errStrC)
	if errStrC != nil {
		C.oglwnd_free_mem(unsafe.Pointer(errStrC))
	}
	return err
}

// Init initializes functions needed to create windows with OpenGL context.
func Init() error {
	if !initialized {
		var errC C.int
		var errWin32C C.oglwnd_ul_t
		C.oglwnd_init(&errC, &errWin32C)
		if errC == 0 {
			initialized = true
			return nil
		}
		return errNumToError(int(errC), uint64(errWin32C), nil)
	}
	return nil
}

// ProcessEvents retrieves messages from thread's message queue for all windows and calls window's handler to process it.
// If no messages available this function returns.
func ProcessEvents() {
	if initialized {
		C.oglwnd_process_events()
	} else {
		panic(notInitialized)
	}
}

// ProcessEventsBlocking retrieves messages from thread's message queue for all windows and calls window's handler to process it.
// This function blocks until further messages are available and returns only if all windows are destroyed.
func ProcessEventsBlocking() {
	if initialized {
		C.oglwnd_process_events_blocking()
	} else {
		panic(notInitialized)
	}
}

// New creates a window with OpenGL 3.0 context and returns it.
func New(params *InitParams) (*Window, error) {
	if initialized {
		var errC C.int
		var errWin32C C.oglwnd_ul_t
		var errStrC *C.char
		if params == nil {
			params = new(InitParams)
			params.Init()
		}
		x := C.int(params.ClientX)
		y := C.int(params.ClientY)
		w := C.int(params.ClientWidth)
		h := C.int(params.ClientHeight)
		wn := C.int(params.ClientMinWidth)
		hn := C.int(params.ClientMinHeight)
		wx := C.int(params.ClientMaxWidth)
		hx := C.int(params.ClientMaxHeight)
		c := boolToCInt(params.Centered)
		l := boolToCInt(params.MouseLocked)
		b := boolToCInt(params.Borderless)
		d := boolToCInt(params.Dragable)
		r := boolToCInt(params.Resizable)
		f := boolToCInt(params.Fullscreen)
		window := new(Window)
		window.handler = params.handler
		window.threaded = params.Threaded
		window.updatable = params.Updatable
		C.oglwnd_window_new(&window.data, unsafe.Pointer(window), x, y, w, h, wn, hn, wx, hx, b, d, r, f, l, c, &errC, &errWin32C, &errStrC)
		C.oglwnd_window_init(window.data, &errC, &errWin32C, &errStrC)
		if errC == 0 {
			return window, nil
		}
		err := errNumToError(int(errC), uint64(errWin32C), errStrC)
		if errStrC != nil {
			C.oglwnd_free_mem(unsafe.Pointer(errStrC))
		}
		return nil, err
	}
	panic(notInitialized)
}

// Quit stops the handler thread, if available, closes
// the window and releases all ressources associated with it.
func (window *Window) Quit() {
	C.oglwnd_window_destroy(window.data)
}

// Context returns the window's OpenGL context.
func (window *Window) Context() Context {
	return &tContext{window.data}
}

// errNumToError converts an error number to a Go error object.
func errNumToError(err int, errWin32 uint64, errStrC *C.char) error {
	if err != 0 {
		var errStr string
		switch err {
		case 1:
			errStr = "get module instance failed (1)"
		case 2:
			errStr = "register dummy class failed (2)"
		case 3:
			errStr = "create dummy window failed (3)"
		case 4:
			errStr = "get dummy device context failed (4)"
		case 5:
			errStr = "choose dummy pixel format failed (5)"
		case 6:
			errStr = "set dummy pixel format failed (6)"
		case 7:
			errStr = "create dummy render context failed (7)"
		case 8:
			errStr = "make dummy context current failed (8)"
		case 9:
			errStr = "get wglChoosePixelFormatARB failed (9)"
		case 10:
			errStr = "get wglCreateContextAttribsARB failed (10)"
		case 11:
			errStr = "register class failed (11)"
		case 12:
			errStr = "create window failed (12)"
		case 13:
			errStr = "get device context failed (13)"
		case 14:
			errStr = "choose pixel format failed (14)"
		case 15:
			errStr = "set pixel format failed (15)"
		case 16:
			errStr = "create render context failed (16)"
		case 17:
			errStr = "make context current failed (17)"
		case 18:
			errStr = "release context failed (18)"
		case 19:
			errStr = "swap buffer failed (19)"
		default:
			errStr = "oglwnd: unknown error"
		}
		if errWin32 > 0 {
			errStr = errStr + " - " + uint64ToString(errWin32)
		}
		if errStrC != nil {
			errStr = errStr + "; " + C.GoString(errStrC)
		}
		return errors.New(errStr)
	}
	return nil
}
