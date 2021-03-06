# oglwnd

[![GoDoc](https://godoc.org/github.com/vbsw/oglwnd?status.svg)](https://godoc.org/github.com/vbsw/oglwnd) [![Stability: Experimental](https://masterminds.github.io/stability/experimental.svg)](https://masterminds.github.io/stability/experimental.html)

## About
oglwnd is a package for Go to create a window with OpenGL 3.0 context. It is published on <https://github.com/vbsw/oglwnd>.

## Copyright
Copyright 2021, Vitali Baumtrok (vbsw@mailbox.org).

oglwnd is distributed under the Boost Software License, version 1.0. (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

oglwnd is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the Boost Software License for more details.

## Compile
Install Go (https://golang.org/doc/install). For Cgo install a C compiler (<https://jmeubank.github.io/tdm-gcc/>).

	// example
	package main
	import "github.com/vbsw/oglwnd"
	import "fmt"

	type myHandler struct {
		oglwnd.DefaultHandler
	}

	func (h *myHandler) OnUpdate(window *oglwnd.Window) error {
		oglwnd.Clear(0.2, 0.2, 0.2, 0)
		return nil
	}

	func main() {
		err := oglwnd.Start(new(myHandler))
		if err == nil {
			fmt.Println(err.Error())
		}
	}

To compile without console on windows use

	-ldflags -H=windowsgui

## References
- https://golang.org/doc/install
- https://jmeubank.github.io/tdm-gcc/
- https://git-scm.com/book/en/v2/Getting-Started-Installing-Git
- https://dave.cheney.net/2013/10/12/how-to-use-conditional-compilation-with-the-go-build-tool
- https://github.com/golang/go/wiki/cgo
- https://pkg.go.dev/cmd/go#hdr-Compile_packages_and_dependencies
- https://pkg.go.dev/cmd/link
- https://www.toomanyatoms.com/computer/usb_keyboard_codes.html
- http://ts.thrustmaster.com/download/accessories/pc/hotas/software/TARGET/TARGET_SCRIPT_EDITOR_basics_v1.2_Appendix.pdf
