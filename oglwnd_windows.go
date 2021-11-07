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
// typedef unsigned long vblong;
import "C"
import (
	"errors"
	"unsafe"
	"fmt"
)

const (
	notInitialized = "oglwnd not initialized"
)

var (
	initialized bool
	bldr        Builder
)

type Properties struct {
	Quit bool
	Visible bool
	window *tWindow
}

type DefaultHandler struct {
}

type tWindow struct {
	data unsafe.Pointer
	handler Handler
	visible bool
}

// Builder is an abstraction of initialization procedures for this package.
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
		C.oglwnd_new_window(bldr.CBuilder(), &window.data, unsafe.Pointer(window), &errNum, &errStrExtra)
		err := bldr.Error(errNum, errStrExtra)
		if err != nil {
			window.destroy()
			C.oglwnd_free_mem(unsafe.Pointer(errStrExtra))
			window = nil
		}
		return window, err
	}
	panic(notInitialized)
}

func ProcessEvents() {
	if initialized {
		C.oglwnd_process_events();
	} else {
		panic(notInitialized)
	}
}

func ProcessEventsBlocking() {
	if initialized {
		C.oglwnd_process_events_blocking();
	} else {
		panic(notInitialized)
	}
}

func (props *Properties) Apply() {
	props.window.applyProps(props)
}

func (handler *DefaultHandler) OnCloseRequest(props *Properties) {
	props.Quit = true
	props.Apply()
}

func (window *tWindow) ProcessEvents() {
	C.oglwnd_process_window_events(window.data)
}

func (window *tWindow) ProcessEventsBlocking() {
	C.oglwnd_process_window_events_blocking(window.data)
}

func (window *tWindow) Show() {
	window.visible = true
	C.oglwnd_show(window.data)
}

func (window *tWindow) Valid() bool {
	return window.data != nil
}

func (window *tWindow) Props() *Properties {
	props := new(Properties)
	props.window = window
	return props
}

func (window *tWindow) applyProps(props *Properties) {
	if props.Quit {
		window.destroy()
	} else {
		if window.visible != props.Visible {
			if props.Visible {
				window.Show()
			}
		}
	}
}

func (window *tWindow) destroy() {
	C.oglwnd_destroy_window(window.data)
	window.data = nil
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

// Error creates and returns an error from errNum. Parameter errStrExtra is appended, if it isn't nil.
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
	window.handler.OnCloseRequest(window.Props())
}

//export goDebug
func goDebug(a, b C.int, c, d C.vblong) {
	fmt.Println(a, b, c, d)
}
