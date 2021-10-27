/*
 *          Copyright 2021, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

package oglwnd

// #cgo CFLAGS: -D_OGLWND_WIN32
// #cgo LDFLAGS: -luser32 -lgdi32 -lOpenGL32
// #include "oglwnd.h"
import "C"
import (
	"errors"
)

var (
	menu, resizing, dragging, minimized bool
)

// Start creates a window with OpenGL 3.0 context and starts the main loop.
func Start(handler Handler) error {
	if !running {
		running = true
		err = nil
		menu, resizing, dragging, minimized = false, false, false, false
		hn = handler
		initOGLWindow()
		if err == nil {
			err = handler.OnOpenGLInit()
			if err == nil {
				C.oglwnd_show()
				C.oglwnd_main_loop()
			}
			destroyWindow()
		}
		running = false
		return err
	}
	return nil
}

// Clear clears the background with given color. (For test purpose.)
func Clear(r, g, b, a float32) {
	if running {
		C.oglwnd_clear(C.float(r), C.float(g), C.float(b), C.float(a))
	}
}

func initOGLWindow() {
	var config Config
	config.Width = 640
	config.Height = 480
	config.MaxWidth = 99999
	config.MaxHeight = 99999
	config.Centered = true
	config.Borderless = false
	config.Dragable = false
	config.Resizable = true
	config.Fullscreen = false
	config.MouseLocked = false
	err = hn.OnConfig(&config)
	if err == nil {
		var errNum C.int
		x := C.int(config.X)
		y := C.int(config.Y)
		w := C.int(int(config.Width))
		h := C.int(int(config.Height))
		wMin := C.int(int(config.MinWidth))
		hMin := C.int(int(config.MinHeight))
		wMax := C.int(int(config.MaxWidth))
		hMax := C.int(int(config.MaxHeight))
		c := boolToCInt(config.Centered)
		b := boolToCInt(config.Borderless)
		d := boolToCInt(config.Dragable)
		r := boolToCInt(config.Resizable)
		f := boolToCInt(config.Fullscreen)
		l := boolToCInt(config.MouseLocked)
		C.oglwnd_init(x, y, w, h, wMin, hMin, wMax, hMax, c, b, d, r, f, l, &errNum)
		err = errNumToError(int(errNum))
	}
}

func destroyWindow() {
	C.oglwnd_destroy()
}

func errNumToError(errNum int) error {
	if errNum != 0 {
		switch errNum {
		case 1:
			return errors.New("get module instance failed")
		case 2:
			return errors.New("register dummy class failed")
		case 3:
			return errors.New("create dummy window failed")
		case 4:
			return errors.New("get dummy device context failed")
		case 5:
			return errors.New("choose dummy pixel format failed")
		case 6:
			return errors.New("set dummy pixel format failed")
		case 7:
			return errors.New("create dummy render context failed")
		case 8:
			return errors.New("make dummy context current failed")
		case 9:
			return errors.New("get wglChoosePixelFormatARB failed")
		case 10:
			return errors.New("get wglCreateContextAttribsARB failed")
		case 11:
			return errors.New("register class failed")
		case 12:
			return errors.New("create window failed")
		case 13:
			return errors.New("get device context failed")
		case 14:
			return errors.New("choose pixel format failed")
		case 15:
			return errors.New("set pixel format failed")
		case 16:
			return errors.New("create render context failed")
		case 17:
			return errors.New("make context current failed")
		}
		return errors.New("oglwnd: unknown error")
	}
	return nil
}

func updateWindowStruct() {
	var x, y, w, h, wMin, hMin C.int
	var wMax, hMax, b, d, r, f, l C.int
	C.oglwnd_get_window_props(&x, &y, &w, &h, &wMin, &hMin, &wMax, &hMax, &b, &d, &r, &f, &l)
	window.ClientX = int(x)
	window.ClientY = int(y)
	window.ClientW = uint(w)
	window.ClientH = uint(h)
	window.MinWidth = uint(wMin)
	window.MinHeight = uint(hMin)
	window.MaxWidth = uint(wMax)
	window.MaxHeight = uint(hMax)
	window.Borderless = bool(b != 0)
	window.Dragable = bool(d != 0)
	window.Resizable = bool(r != 0)
	window.Fullscreen = bool(f != 0)
	window.MouseLocked = bool(l != 0)
}

func updateWindow() {
	if err == nil {
		if window.Quit {
			C.oglwnd_stop()
		} else {
			x := C.int(window.ClientX)
			y := C.int(window.ClientY)
			w := C.int(window.ClientW)
			h := C.int(window.ClientH)
			wMin := C.int(window.MinWidth)
			hMin := C.int(window.MinHeight)
			wMax := C.int(window.MaxWidth)
			hMax := C.int(window.MaxHeight)
			b := boolToCInt(window.Borderless)
			d := boolToCInt(window.Dragable)
			r := boolToCInt(window.Resizable)
			f := boolToCInt(window.Fullscreen)
			l := boolToCInt(window.MouseLocked)
			C.oglwnd_set_window_props(x, y, w, h, wMin, hMin, wMax, hMax, b, d, r, f, l)
		}
	} else {
		C.oglwnd_stop()
	}
}

//export goOnClose
func goOnClose() {
	updateWindowStruct()
	err = hn.OnClose(&window)
	updateWindow()
}

//export goOnFirstUpdate
func goOnFirstUpdate() {
	updateWindowStruct()
	err = hn.OnFirstUpdate(&window)
	updateWindow()
}

//export goOnUpdate
func goOnUpdate() {
	updateWindowStruct()
	err = hn.OnUpdate(&window)
	updateWindow()
}

//export goOnKeyDown
func goOnKeyDown(key, repeat C.int) {
	updateWindowStruct()
	err = hn.OnKeyDown(&window, int(key), int(repeat))
	updateWindow()
}

//export goOnKeyUp
func goOnKeyUp(key C.int) {
	updateWindowStruct()
	err = hn.OnKeyUp(&window, int(key))
	updateWindow()
}

//export goOnMove
func goOnMove() {
	updateWindowStruct()
	err = hn.OnMove(&window)
	updateWindow()
}

//export goOnResize
func goOnResize() {
	updateWindowStruct()
	err = hn.OnResize(&window)
	updateWindow()
}

//export goOnFirstWindowSize
func goOnFirstWindowSize() {
	updateWindowStruct()
	err = hn.OnFirstWindowSize(&window)
	updateWindow()
}

//export goOnMenuEnter
func goOnMenuEnter() {
	if !resizing && !dragging && !minimized {
		updateWindowStruct()
		err = hn.OnUpdateStop(&window)
		updateWindow()
	}
	menu = true
}

//export goOnMenuLeave
func goOnMenuLeave() {
	if !resizing && !dragging && !minimized {
		updateWindowStruct()
		err = hn.OnUpdateContinue(&window)
		updateWindow()
	}
	menu = false
}

//export goOnMaximize
func goOnMaximize() {
}

//export goOnMinimize
func goOnMinimize() {
	if !menu && !resizing && !dragging {
		updateWindowStruct()
		err = hn.OnUpdateStop(&window)
		updateWindow()
	}
	minimized = true
}

//export goOnRestore
func goOnRestore() {
	if !menu && !resizing && !dragging {
		updateWindowStruct()
		err = hn.OnUpdateContinue(&window)
		updateWindow()
	}
	minimized = false
}

//export goOnFocusLoose
func goOnFocusLoose() {
	updateWindowStruct()
	err = hn.OnFocusLoose(&window)
	updateWindow()
}

//export goOnFocusGain
func goOnFocusGain() {
	updateWindowStruct()
	err = hn.OnFocusGain(&window)
	updateWindow()
}

//export goOnMouseMove
func goOnMouseMove() {
	updateWindowStruct()
	err = hn.OnMouseMove(&window)
	updateWindow()
}

//export goOnDragBegin
func goOnDragBegin() {
	if !menu && !resizing && !minimized {
		updateWindowStruct()
		err = hn.OnUpdateStop(&window)
		updateWindow()
	}
	dragging = true
}

//export goOnDragEnd
func goOnDragEnd() {
	if !menu && !resizing && !minimized {
		updateWindowStruct()
		err = hn.OnUpdateContinue(&window)
		updateWindow()
	}
	dragging = false
}

//export goOnDragCustBegin
func goOnDragCustBegin() {
}

//export goOnDragCustEnd
func goOnDragCustEnd() {
}

//export goOnResizeBegin
func goOnResizeBegin() {
	if !menu && !dragging && !minimized {
		updateWindowStruct()
		err = hn.OnUpdateStop(&window)
		updateWindow()
	}
	resizing = true
}

//export goOnResizeEnd
func goOnResizeEnd() {
	if !menu && !dragging && !minimized {
		updateWindowStruct()
		err = hn.OnUpdateContinue(&window)
		updateWindow()
	}
	resizing = false
}

//export goOnButtonDown
func goOnButtonDown(button, doubleClick C.int) {
	updateWindowStruct()
	err = hn.OnButtonDown(&window, int(button), bool(doubleClick != 0))
	updateWindow()
}

//export goOnButtonUp
func goOnButtonUp(button C.int) {
	updateWindowStruct()
	err = hn.OnButtonUp(&window, int(button))
	updateWindow()
}

//export goOnWheel
func goOnWheel(wheel C.float) {
	updateWindowStruct()
	err = hn.OnWheel(&window, float32(wheel))
	updateWindow()
}
