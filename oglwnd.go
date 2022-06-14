/*
 *          Copyright 2022, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

// Package oglwnd creates a window with OpenGL 3.0 context.
package oglwnd

import "C"
import (
	"unsafe"
)

const (
	notAllocated = "window's C memory not allocated"
	notInitialized = "window not initialized"
	notInitializedCtx = "opengl context not initialized"
	notInitializedFuncs = "window functions not initialized"
	notInitializedDtFunc = "window destroy function not initialized"
)

type Window struct {
	Ptr unsafe.Pointer
	Initialized bool
}

// Context represents OpenGL context.
type Context interface {
	MakeCurrent() error
	Release() error
	SwapBuffers() error
}

// toCInt converts bool value to C int value.
func toCInt(b bool) C.int {
	if b {
		return C.int(1)
	}
	return C.int(0)
}

/*
import "C"

const (
	KeyA           = 4
	KeyB           = 5
	KeyC           = 6
	KeyD           = 7
	KeyE           = 8
	KeyF           = 9
	KeyG           = 10
	KeyH           = 11
	KeyI           = 12
	KeyJ           = 13
	KeyK           = 14
	KeyL           = 15
	KeyM           = 16
	KeyN           = 17
	KeyO           = 18
	KeyP           = 19
	KeyQ           = 20
	KeyR           = 21
	KeyS           = 22
	KeyT           = 23
	KeyU           = 24
	KeyV           = 25
	KeyW           = 26
	KeyX           = 27
	KeyY           = 28
	KeyZ           = 29
	Key1           = 30
	Key2           = 31
	Key3           = 32
	Key4           = 33
	Key5           = 34
	Key6           = 35
	Key7           = 36
	Key8           = 37
	Key9           = 38
	Key0           = 39
	KeyEnter       = 40
	KeyEsc         = 41
	KeyBackspace   = 42
	KeyTab         = 43
	KeySpace       = 44
	KeyMinus       = 45
	KeyEqual       = 46
	KeyLBrace      = 47
	KeyRBrace      = 48
	KeyBSlash      = 49
	KeyTilde       = 50
	KeySemicolon   = 51
	KeyApostrophe  = 52
	KeyGrave       = 53
	KeyComma       = 54
	KeyDot         = 55
	KeySlash       = 56
	KeyCaps        = 57
	KeyF1          = 58
	KeyF2          = 59
	KeyF3          = 60
	KeyF4          = 61
	KeyF5          = 62
	KeyF6          = 63
	KeyF7          = 64
	KeyF8          = 65
	KeyF9          = 66
	KeyF10         = 67
	KeyF11         = 68
	KeyF12         = 69
	KeyScroll      = 71
	KeyPause       = 72
	KeyInsert      = 73
	KeyHome        = 74
	KeyPageUp      = 75
	KeyDelete      = 76
	KeyEnd         = 77
	KeyPageDown    = 78
	KeyRight       = 79
	KeyLeft        = 80
	KeyDown        = 81
	KeyUp          = 82
	KeyNumLock     = 83
	KeyPadSlash    = 84
	KeyPadAsterisk = 85
	KeyPadMinus    = 86
	KeyPadPlus     = 87
	KeyPadEnter    = 88
	KeyPad1        = 89
	KeyPad2        = 90
	KeyPad3        = 91
	KeyPad4        = 92
	KeyPad5        = 93
	KeyPad6        = 94
	KeyPad7        = 95
	KeyPad8        = 96
	KeyPad9        = 97
	KeyPad0        = 98
	KeyPadDot      = 99
	KeyPipe        = 100
	KeyPadEqual    = 103
	KeyMenu        = 118
	KeyPadComma    = 133
	KeyLCtrl       = 224
	KeyLShift      = 225
	KeyLAlt        = 226
	KeyRCtrl       = 228
	KeyRShift      = 229
	KeyRAlt        = 230
)

var (
	initialized bool
)

// InitParams is the window's initialization parameters.
type InitParams struct {
	ClientX, ClientY, ClientWidth, ClientHeight                      int
	ClientMinWidth, ClientMinHeight, ClientMaxWidth, ClientMaxHeight int
	handler                                                          Handler
	Centered, MouseLocked, Borderless, Dragable                      bool
	Resizable, Fullscreen, Threaded, Updatable                       bool
}

// Context provides OpenGL context functions.
type Context interface {
	MakeCurrent() error
	Release() error
	SwapBuffers() error
}

// Props holds properties of the window. Used for checking and updating the window.
type Props struct {
	ClientX, ClientY, ClientWidth, ClientHeight                      int
	ClientMinWidth, ClientMinHeight, ClientMaxWidth, ClientMaxHeight int
	MouseX, MouseY                                                   int
	MouseLocked, Borderless, Dragable, Resizable, Fullscreen         bool
	Quit                                                             bool
}

// Handler is an abstraction of event handling.
type Handler interface {
	OnClose(*Props) error
	OnUpdate(*Props) error
}

// DefaultHandler is the default handling of events.
type DefaultHandler struct {
}

// Init sets the initialization parameters to default values.
func (params *InitParams) Init() {
	params.ClientWidth = 640
	params.ClientHeight = 480
	params.ClientMaxWidth = 99999
	params.ClientMaxHeight = 99999
	params.Centered = true
	params.Resizable = true
}

// OnClose is called at window's close request.
func (hand *DefaultHandler) OnClose(props *Props) error {
	props.Quit = true
	return nil
}

// OnUpdate is called after all window's events has been processed.
// This function must be enabled at window's creation.
func (hand *DefaultHandler) OnUpdate(props *Props) error {
	return nil
}

// uint64ToString converts a positive integer value to string.
func uint64ToString(value uint64) string {
	var byteArr [20]byte
	var decimals int
	tenth := value / 10
	byteArr[19] = byte(value - tenth*10 + 48)
	value = tenth
	for decimals = 1; value > 0 && decimals < 20; decimals++ {
		tenth := value / 10
		byteArr[19-decimals] = byte(value - tenth*10 + 48)
		value = tenth
	}
	return string(byteArr[20-decimals:])
}
*/