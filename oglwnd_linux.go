/*
 *          Copyright 2021, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

// Package oglwnd creates windows with OpenGL context.
package oglwnd

// #cgo CFLAGS: -DOGLWND_X
// #include "oglwnd.h"
import "C"
import (
	"errors"
	"unsafe"
)

// Builder is an abstraction of initialization procedures for this package.
type Builder interface {
	InitCBuilder() unsafe.Pointer
	DestroyCBuilder(*C.char)
	Error(C.int, *C.char) error
}

// DefaultBuilder is the default implementation of Builder interface.
type DefaultBuilder struct {
	cbuilder unsafe.Pointer
}

var (
	initialized bool
	bldr        Builder
)

// SetBuilder sets a custom builder. If builder is nil, default builder is used.
func SetBuilder(builder Builder) {
	if bldr != nil {
		bldr.DestroyCBuilder(nil)
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
		cbuilder := bldr.InitCBuilder()
		C.oglwnd_init(cbuilder, &errNum, &errStrExtra)
		err := bldr.Error(errNum, errStrExtra)
		if err == nil {
			initialized = true
		} else {
			bldr.DestroyCBuilder(errStrExtra)
		}
		return err
	}
	return nil
}

// Destroy releases memory allocated by Init function.
func Destroy() {
	if initialized {
		C.oglwnd_destroy()
		bldr.DestroyCBuilder(nil)
		initialized = false
	}
}

// InitCBuilder initializes an instance of C.builder_t. Returns pointer to it.
func (builder *DefaultBuilder) InitCBuilder() unsafe.Pointer {
	C.oglwnd_new_builder(&builder.cbuilder)
	return builder.cbuilder
}

// DestroyCBuilder releases all memory associated with cbuilder and string.
func (builder *DefaultBuilder) DestroyCBuilder(cstr *C.char) {
	C.oglwnd_destroy_builder(builder.cbuilder, cstr)
	builder.cbuilder = nil
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
