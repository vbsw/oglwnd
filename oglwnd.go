/*
 *          Copyright 2021, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

// Package oglwnd creates a window with OpenGL 3.0 context.
package oglwnd

import "C"

var (
	running bool
	err     error
	hn      Handler
	window  Window
)

type Config struct {
	X, Y, Width, Height   uint
	WidthMin, HeightMin   uint
	WidthMax, HeightMax   uint
	Quit, Centered        bool
	Borderless, Dragable  bool
	Resizable, Fullscreen bool
}

type Window struct {
	X, Y, Width, Height uint
	WidthMin, HeightMin uint
	WidthMax, HeightMax uint
	Quit                bool
}

type Handler interface {
	OnConfig(config *Config) error
	OnClose(window *Window) error
	OnOpenGLInit() error
	OnFirstUpdate(window *Window) error
	OnUpdate(window *Window) error
}

type DefaultHandler struct {
}

func (dhn *DefaultHandler) OnConfig(config *Config) error {
	return nil
}

func (dhn *DefaultHandler) OnClose(window *Window) error {
	window.Quit = true
	return nil
}

func (dhn *DefaultHandler) OnOpenGLInit() error {
	return nil
}

func (dhn *DefaultHandler) OnFirstUpdate(window *Window) error {
	return nil
}

func (dhn *DefaultHandler) OnUpdate(window *Window) error {
	return nil
}

func boolToCInt(b bool) C.int {
	if b {
		return C.int(1)
	}
	return C.int(0)
}
