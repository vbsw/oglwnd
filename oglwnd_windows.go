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

// Show creates a window with OpenGL 3.0 context and starts the main loop.
func Show(handler Handler) error {
	if !running {
		running = true
		err = nil
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

func initOGLWindow() {
	var config Config
	config.X = 0
	config.Y = 0
	config.Width = 640
	config.Height = 480
	config.WidthMax = 99999
	config.HeightMax = 99999
	config.Centered = true
	config.Borderless = false
	config.Dragable = false
	config.Resizable = true
	config.Fullscreen = false
	err = hn.OnConfig(&config)
	if err == nil {
		var errNum C.int
		x := C.int(int(config.X))
		y := C.int(int(config.Y))
		w := C.int(int(config.Width))
		h := C.int(int(config.Height))
		wMin := C.int(int(config.WidthMin))
		hMin := C.int(int(config.HeightMin))
		wMax := C.int(int(config.WidthMax))
		hMax := C.int(int(config.HeightMax))
		c := boolToCInt(config.Centered)
		b := boolToCInt(config.Borderless)
		d := boolToCInt(config.Dragable)
		r := boolToCInt(config.Resizable)
		f := boolToCInt(config.Fullscreen)
		C.oglwnd_init(x, y, w, h, wMin, hMin, wMax, hMax, c, b, d, r, f, &errNum)
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
	}
	return nil
}

func updateWindowStruct() {
	/* TODO */
}

func updateWindow() {
	if err == nil {
		if window.Quit {
			C.oglwnd_stop()
		} else {
			/* TODO */
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
