//------------------------------------------------------------------------------
// png2array.go
//------------------------------------------------------------------------------

package main

import (
	"flag"
	"fmt"
	"image"
	"image/png"
	//"io"
	"log"
	"os"
)

func main() {
	var err error

	// support png only
	image.RegisterFormat("png", "png", png.Decode, png.DecodeConfig)

	// parse args and extract mode string
	var mode string
	flag.StringVar(&mode, "m", "rgb565", "encoding mode (e.g. rgb565)")
	flag.Parse()
	if len(flag.Args()) < 1 {
		log.Printf("usage : ./png2array [-m {encoding mode}] {input image file}\n")
		os.Exit(1)
	}

	// open input image
	var file_in *os.File
	file_in, err = os.Open(flag.Args()[0])
	if err != nil {
		log.Printf("failed to open '%s'\n", flag.Args()[0])
		os.Exit(1)
	}
	defer file_in.Close()

	// decode
	var img image.Image
	img, _, err = image.Decode(file_in)
	if err != nil {
		log.Printf("failed to decode '%s'", flag.Args()[0])
		os.Exit(1)
	}

	// scan pixels and convert
	var width int
	var height int
	width = img.Bounds().Max.X
	height = img.Bounds().Max.Y

	for y := 0; y < height; y++ {
		for x := 0; x < width; x++ {
			var r uint32
			var g uint32
			var b uint32
			var rgb565 uint16
			r, g, b, _ = img.At(x, y).RGBA()
			r = uint32((r / 257) >> 3)
			g = uint32((g / 257) >> 2)
			b = uint32((b / 257) >> 3)
			rgb565 = uint16(r<<11 + g<<5 + b)

			fmt.Printf("0x%02x, 0x%02x, ", (rgb565 & 0x00FF), (rgb565 >> 8))
		}
		fmt.Printf("\n")
	}
}
