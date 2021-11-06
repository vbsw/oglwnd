/*
 *          Copyright 2021, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

// Package oglwnd creates windows with OpenGL context.
package oglwnd

// #cgo CFLAGS: -D_OGLWND_WIN32
// #cgo LDFLAGS: -luser32 -lgdi32 -lOpenGL32
// #include "oglwnd.h"
import "C"
import (
	"errors"
	"unsafe"
)

// Builder is an abstraction of initialization procedures for this package.
type Builder interface {
	NewCBuilder() unsafe.Pointer
	DestroyCBuilder(unsafe.Pointer, *C.char)
	Error(C.int, *C.char) error
}

// DefaultBuilder is the default implementation of Builder interface.
type DefaultBuilder struct {
}

var (
	initialized   bool
	customBuilder Builder
)

// SetBuilder sets a custom builder. If builder is nil, default builder is used.
func SetBuilder(builder Builder) {
	customBuilder = builder
}

// Init initializes functions and data needed for this package.
func Init() error {
	if !initialized {
		var errNum C.int
		var errStrExtra *C.char
		builder := customBuilder
		if builder == nil {
			builder = new(DefaultBuilder)
		}
		cbuilder := builder.NewCBuilder()
		C.oglwnd_init(cbuilder, &errNum, &errStrExtra)
		err := builder.Error(errNum, errStrExtra)
		builder.DestroyCBuilder(cbuilder, errStrExtra)
		initialized = bool(err == nil)
		return err
	}
	return nil
}

// Destroy releases memory allocated by Init function.
func Destroy() {
	if initialized {
		C.oglwnd_destroy()
		initialized = false
	}
}

// NewCBuilder creates and returns a new instance of DefaultBuilder.
func (builder *DefaultBuilder) NewCBuilder() unsafe.Pointer {
	var cbuilder unsafe.Pointer
	C.oglwnd_new_builder(&cbuilder)
	return cbuilder
}

// DestroyCBuilder releases all memory allocated in C by DefaultBuilder.
func (builder *DefaultBuilder) DestroyCBuilder(cbuilder unsafe.Pointer, cstr *C.char) {
	C.oglwnd_destroy_builder(cbuilder, cstr)
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
			errStr = "get wglChoosePixelFormatARB failed"
		case 10:
			errStr = "get wglCreateContextAttribsARB failed"
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
