/*
 *          Copyright 2021, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

// Package oglwnd creates windows with OpenGL context.
package oglwnd

// #cgo CFLAGS: -D_OGLWND_X
// #include "oglwnd.h"
import "C"

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

// Init initializes functions and data needed for this package.
func Init() error {
	// TODO
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
