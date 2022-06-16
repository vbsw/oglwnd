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
	"strconv"
	"unsafe"
)

// Parameters is the window's initialization parameters.
type Parameters struct {
	Dummy bool
	ClientX, ClientY, ClientWidth, ClientHeight                      int
	ClientMinWidth, ClientMinHeight, ClientMaxWidth, ClientMaxHeight int
	Centered, MouseLocked, Borderless, Dragable                      bool
	Resizable, Fullscreen, Threaded, AutoUpdate                      bool
}

type tContext struct {
	ptr unsafe.Pointer
}

// ProcessEvents retrieves messages from thread's message queue for all windows.
// This function blocks until further messages are available and returns only if all windows are destroyed.
func ProcessEvents() {
	C.oglwnd_process_events()
}

// Reset sets the initialization parameters to default values.
func (params *Parameters) Reset() {
	params.Dummy = false
	params.ClientX = 0
	params.ClientY = 0
	params.ClientWidth = 640
	params.ClientHeight = 480
	params.ClientMinWidth = 0
	params.ClientMinHeight = 0
	params.ClientMaxWidth = 99999
	params.ClientMaxHeight = 99999
	params.Centered = true
	params.MouseLocked = false
	params.Borderless = false
	params.Dragable = false
	params.Resizable = true
	params.Fullscreen = false
	params.Threaded = false
	params.AutoUpdate = false
}

func (context *tContext) MakeCurrent() error {
	var errC unsafe.Pointer
	if context.ptr != nil {
		C.oglwnd_context_make_current(context.ptr, &errC)
	} else {
		panic(notInitializedCtx)
	}
	return toError(errC)
}

func (context *tContext) Release() error {
	var errC unsafe.Pointer
	if context.ptr != nil {
		C.oglwnd_context_release(context.ptr, &errC)
	} else {
		panic(notInitializedCtx)
	}
	return toError(errC)
}

func (context *tContext) SwapBuffers() error {
	var errC unsafe.Pointer
	if context.ptr != nil {
		C.oglwnd_context_swap_buffers(context.ptr, &errC)
	} else {
		panic(notInitializedCtx)
	}
	return toError(errC)
}

func (wnd *Window) Init(params *Parameters) error {
	var err error
	if wnd.Ptr != nil {
		params = ensureParams(params)
		err = wnd.init(params)
		wnd.Initialized = bool(err == nil)
	} else {
		panic(notAllocated)
	}
	return err
}

func (wnd *Window) Create() error {
	var errC unsafe.Pointer
	if wnd.Ptr != nil {
		if wnd.Initialized {
			if C.oglwnd_window_funcs_avail(wnd.Ptr) == 1 {
				C.oglwnd_window_create(wnd.Ptr, &errC)
			} else {
				panic(notInitializedFuncs)
			}
		} else {
			panic(notInitialized)
		}
	} else {
		panic(notAllocated)
	}
	return toError(errC)
}

func (wnd *Window) Context() Context {
	var ctx *tContext
	if wnd.Ptr != nil {
		var ctxPtr unsafe.Pointer
		ctx = new(tContext)
		C.oglwnd_window_context(wnd.Ptr, &ctxPtr)
		ctx.ptr = ctxPtr
	} else {
		panic(notAllocated)
	}
	return ctx
}

// Show makes window visible.
func (wnd *Window) Show() error {
	var errC unsafe.Pointer
	if wnd.Ptr != nil {
		C.oglwnd_window_show(wnd.Ptr, &errC)
	} else {
		panic(notAllocated)
	}
	return toError(errC)
}

// Destroy
func (wnd *Window) Destroy() error {
	var errC unsafe.Pointer
	if wnd.Ptr != nil {
		if wnd.Initialized {
			if C.oglwnd_window_dt_func_avail(wnd.Ptr) == 1 {
				C.oglwnd_window_destroy(wnd.Ptr, &errC)
			} else {
				panic(notInitializedDtFunc)
			}
		}
	} else {
		panic(notAllocated)
	}
	return toError(errC)
}

// ReleaseMemory deallocates memory Ptr points to.
func (wnd *Window) ReleaseMemory() error {
	var errC unsafe.Pointer
	if wnd.Ptr != nil {
		C.oglwnd_window_free(wnd.Ptr, &errC)
		wnd.Ptr = nil
	}
	return toError(errC)
}

func (wnd *Window) Allocate() error {
	var errC unsafe.Pointer
	C.oglwnd_window_allocate(&wnd.Ptr, &errC)
	return toError(errC)
}

func (wnd *Window) init(params *Parameters) error {
	var errC unsafe.Pointer
	if params.Dummy {
		C.oglwnd_window_init_dummy(wnd.Ptr, &errC)
	} else {
		x := C.int(params.ClientX)
		y := C.int(params.ClientY)
		w := C.int(params.ClientWidth)
		h := C.int(params.ClientHeight)
		wn := C.int(params.ClientMinWidth)
		hn := C.int(params.ClientMinHeight)
		wx := C.int(params.ClientMaxWidth)
		hx := C.int(params.ClientMaxHeight)
		c := toCInt(params.Centered)
		l := toCInt(params.MouseLocked)
		b := toCInt(params.Borderless)
		d := toCInt(params.Dragable)
		r := toCInt(params.Resizable)
		f := toCInt(params.Fullscreen)
		objC := C.int(cb.Register(wnd))
		C.oglwnd_window_init_opengl30(wnd.Ptr, objC, x, y, w, h, wn, hn, wx, hx, b, d, r, f, l, c, &errC)
	}
	return toError(errC)
}

func ensureParams(params *Parameters) *Parameters {
	if params == nil {
		params = new(Parameters)
		params.Reset()
	}
/*
	if params.handler == nil {
		params.handler = new(DefaultHandler)
	}
*/
	return params
}

// toError converts C error to Go error.
func toError(errC unsafe.Pointer) error {
	if errC != nil {
		var errStr string
		var errNumC C.int
		var errWin32 C.oglwnd_ul_t
		var errStrC *C.char
		C.oglwnd_error(errC, &errNumC, &errWin32, &errStrC)
		switch errNumC {
		case 1:
			errStr = "allocate memory failed"
		case 2:
			errStr = "get module instance failed"
		case 3:
			errStr = "register dummy class failed"
		case 4:
			errStr = "create dummy window failed"
		case 5:
			errStr = "get dummy device context failed"
		case 6:
			errStr = "choose dummy pixel format failed"
		case 7:
			errStr = "set dummy pixel format failed"
		case 8:
			errStr = "create dummy render context failed"
		case 10:
			errStr = "release dummy context failed"
		case 11:
			errStr = "deleting dummy render context failed"
		case 12:
			errStr = "destroying dummy window failed"
		case 13:
			errStr = "unregister dummy class failed"
		case 14:
			errStr = "swap dummy buffer failed"
		case 15:
			errStr = "window functions not initialized"
		case 50:
			errStr = "register class failed"
		case 51:
			errStr = "create window failed"
		case 52:
			errStr = "get device context failed"
		case 53:
			errStr = "choose pixel format failed"
		case 54:
			errStr = "set pixel format failed"
		case 55:
			errStr = "create render context failed"
		case 56:
			errStr = "make context current failed"
		case 57:
			errStr = "release context failed"
		case 58:
			errStr = "deleting render context failed"
		case 59:
			errStr = "destroying window failed"
		case 60:
			errStr = "unregister class failed"
		case 61:
			errStr = "swap buffer failed"
		case 62:
			errStr = "set title failed"
		case 63:
			errStr = "wgl functions not initialized"
		default:
			errStr = "unknown error " + strconv.FormatUint(uint64(errNumC), 10)
		}
		if errWin32 != 0 {
			errStr = errStr + " (" + strconv.FormatUint(uint64(errWin32), 10) + ")"
		}
		if errStrC != nil {
			errStr = errStr + "; " + C.GoString(errStrC)
		}
		C.oglwnd_error_free(errC)
		return errors.New(errStr)
	}
	return nil
}

//export goOnClose
func goOnClose(objIdC C.int) {
	if wnd, ok := cb.Obj(int(objIdC)).(*Window); ok {
		println("close")
		wnd.Destroy()
	}
/*
	if window, ok := hndlValue(int(hndlC)).(*Window); ok {
		window.updateProps()
		propsBak := window.props
		window.err = window.handler.OnClose(&window.props)
		window.props.Destroy = bool(window.err != nil)
		if window.props != propsBak {
			window.applyProps()
		}
	}
*/
}

//export goDebug
func goDebug(a, b C.int, c, d C.oglwnd_ul_t) {
	println(a, b, c, d)
}

/*

// Window is a window with OpenGL context.
type Window struct {
	data                unsafe.Pointer
	handler             Handler
	threaded, updatable bool
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
*/