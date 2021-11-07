/*
 *          Copyright 2021, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

package oglwnd

// #cgo CFLAGS: -DOGLWND_WIN32 -DUNICODE
// #cgo LDFLAGS: -luser32 -lgdi32 -lOpenGL32
// #include "oglwnd.h"
// typedef unsigned long vblong;
import "C"
import (
	"errors"
	"fmt"
	"unsafe"
)

const (
	notInitialized = "oglwnd not initialized"
)

var (
	initialized bool
	bldr        Builder
)

// Properties of the window.
type Properties struct {
	window *tWindow
}

// DefaultHandler is the default implementation of Hanlder interface.
type DefaultHandler struct {
}

type tWindow struct {
	data    unsafe.Pointer
	handler Handler
	visible bool
	title   string
}

// Builder is an abstraction of initialization for this package.
type Builder interface {
	NewCBuilder() unsafe.Pointer
	DestroyCBuilder()
	CBuilder() unsafe.Pointer
	Error(C.int, *C.char) error
}

// DefaultBuilder is the default implementation of Builder interface.
type DefaultBuilder struct {
	cbuilder unsafe.Pointer
}

// SetBuilder sets a custom builder. If builder is nil, default builder is used.
func SetBuilder(builder Builder) {
	if bldr != nil {
		bldr.DestroyCBuilder()
	}
	bldr = builder
}

// Init initializes functions and data needed for this package.
func Init() error {
	if !initialized {
		var errNum C.int
		var errStrExtra *C.char
		if bldr == nil {
			bldr = new(DefaultBuilder)
		}
		cbuilder := bldr.NewCBuilder()
		C.oglwnd_init(cbuilder, &errNum, &errStrExtra)
		err := bldr.Error(errNum, errStrExtra)
		if err == nil {
			initialized = true
		} else {
			bldr.DestroyCBuilder()
			C.oglwnd_free_mem(unsafe.Pointer(errStrExtra))
		}
		return err
	}
	return nil
}

// Destroy releases memory allocated by Init function.
func Destroy() {
	if initialized {
		C.oglwnd_destroy()
		bldr.DestroyCBuilder()
		initialized = false
	}
}

// New returns a new Window object.
func New(handler Handler) (Window, error) {
	if initialized {
		var errNum C.int
		var errStrExtra *C.char
		window := new(tWindow)
		if handler == nil {
			window.handler = new(DefaultHandler)
		} else {
			window.handler = handler
		}
		window.title = "OpenGL"
		C.oglwnd_new_window(bldr.CBuilder(), &window.data, unsafe.Pointer(window), &errNum, &errStrExtra)
		err := bldr.Error(errNum, errStrExtra)
		if err != nil {
			window.Destroy()
			C.oglwnd_free_mem(unsafe.Pointer(errStrExtra))
			window = nil
		}
		return window, err
	}
	panic(notInitialized)
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

// OnCloseRequest is called when a request to close the window has been made.
func (handler *DefaultHandler) OnCloseRequest(window Window) {
	window.Destroy()
}

func (window *tWindow) Destroy() {
	C.oglwnd_destroy_window(window.data)
	window.data = nil
}

// ProcessEvents retrieves messages from thread's message queue for this window and calls its handler to process it.
// If no messages available this function returns.
func (window *tWindow) ProcessEvents() {
	C.oglwnd_process_window_events(window.data)
}

// ProcessEventsBlocking retrieves messages from thread's message queue for this window and calls its handler to process it.
// This function blocks until further messages are available and returns only if this window is destroyed.
func (window *tWindow) ProcessEventsBlocking() {
	C.oglwnd_process_window_events_blocking(window.data)
}

// Props returns properties of the window.
func (window *tWindow) Props() *Properties {
	props := new(Properties)
	props.window = window
	return props
}

// SetTitle sets the title of the window.
func (window *tWindow) SetTitle(title string) {
	if window.title != title {
		window.title = title
		ctitle := C.CString(title)
		C.oglwnd_set_title(window.data, ctitle)
		C.oglwnd_free_mem(unsafe.Pointer(ctitle))
	}
}

// Show makes window visible.
func (window *tWindow) Show() {
	window.visible = true
	C.oglwnd_show(window.data)
}

// Valid returns true, if window is valid, and false otherwise (e.g. after window has been destroyed).
func (window *tWindow) Valid() bool {
	return window.data != nil
}

// NewCBuilder initializes an instance of C.builder_t. Returns pointer to it.
func (builder *DefaultBuilder) NewCBuilder() unsafe.Pointer {
	C.oglwnd_new_builder(&builder.cbuilder)
	return builder.cbuilder
}

// DestroyCBuilder releases all memory associated with cbuilder.
func (builder *DefaultBuilder) DestroyCBuilder() {
	C.oglwnd_destroy_builder(builder.cbuilder)
	builder.cbuilder = nil
}

// CBuilder returns a pointer to C.builder_t
func (builder *DefaultBuilder) CBuilder() unsafe.Pointer {
	return builder.cbuilder
}

// Error creates an error from errNum and returns it. Parameter errStrExtra is appended, if it isn't nil.
func (builder *DefaultBuilder) Error(errNum C.int, errStrExtra *C.char) error {
	if errNum > 0 {
		var errStr string
		switch errNum {
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
			errStr = "load wglChoosePixelFormatARB failed (9)"
		case 10:
			errStr = "load wglCreateContextAttribsARB failed (10)"
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
		default:
			errStr = "unknown"
		}
		if errStrExtra != nil {
			errStr = errStr + " -> " + C.GoString(errStrExtra)
		}
		return errors.New(errStr)
	}
	return nil
}

//export goOnCloseRequest
func goOnCloseRequest(data unsafe.Pointer) {
	window := (*tWindow)(data)
	window.handler.OnCloseRequest(window)
}

//export goDebug
func goDebug(a, b C.int, c, d C.vblong) {
	fmt.Println(a, b, c, d)
}
