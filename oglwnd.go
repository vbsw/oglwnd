/*
 *          Copyright 2021, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

// Package oglwnd creates windows with OpenGL context.
package oglwnd

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

// Window is an interface to control the window.
type Window interface {
	ProcessEvents()
	ProcessEventsBlocking()
	Show()
	Valid() bool
	Props() *Properties
}

// Handler is a callback for window events.
type Handler interface {
	OnCloseRequest(*Properties)
}
