/*
 *          Copyright 2021, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

// Package oglwnd creates windows with OpenGL context.
package oglwnd

// #cgo CFLAGS: -DOGLWND_WIN32 -DUNICODE
// #cgo LDFLAGS: -luser32 -lgdi32 -lOpenGL32
// #include "oglwnd.h"
import "C"
import (
	"errors"
	"unsafe"
)

const notInitialized = "oglwnd not initialized"

var (
	initialized bool
	bldr        Builder
)

type Window interface {
	ProcessEvents()
	ProcessEventsWaiting()
	Destroy()
}

type tWindow struct {
	data unsafe.Pointer
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
func New() (Window, error) {
	if initialized {
		var errNum C.int
		var errStrExtra *C.char
		window := new(tWindow)
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

func ProcessEvents() {
	if initialized {
		C.oglwnd_process_events();
	} else {
		panic(notInitialized)
	}
}

func ProcessEventsWaiting() {
	if initialized {
		C.oglwnd_process_events_waiting();
	} else {
		panic(notInitialized)
	}
}

func (window *tWindow) Destroy() {
	C.oglwnd_destroy_window(window.data)
	window.data = nil
}

func (window *tWindow) ProcessEvents() {
	C.oglwnd_process_window_events(window.data)
}

func (window *tWindow) ProcessEventsWaiting() {
	C.oglwnd_process_window_events_waiting(window.data)
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
			errStr = "get module instance failed"
		case 2:
			errStr = "register dummy class failed"
		case 3:
			errStr = "create dummy window failed"
		case 4:
			errStr = "get dummy device context failed"
		case 5:
			errStr = "choose dummy pixel format failed"
		case 6:
			errStr = "set dummy pixel format failed"
		case 7:
			errStr = "create dummy render context failed"
		case 8:
			errStr = "make dummy context current failed"
		case 9:
			errStr = "load wglChoosePixelFormatARB failed"
		case 10:
			errStr = "load wglCreateContextAttribsARB failed"
		case 11:
			errStr = "register class failed"
		case 12:
			errStr = "create window failed"
		case 13:
			errStr = "get device context failed"
		case 14:
			errStr = "choose pixel format failed"
		case 15:
			errStr = "set pixel format failed"
		case 16:
			errStr = "create render context failed"
		case 17:
			errStr = "make context current failed"
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
