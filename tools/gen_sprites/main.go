package main

import (
	"fmt"
	"image"
	"image/color"
	"image/png"
	"math"
	"os"
	"path/filepath"
)

// ─── Palette ────────────────────────────────────────────────────────────────
// Bush-viper inspired + 80s neon arcade
var (
	// Snake colors - inspired by bush viper reference
	snakeTeal      = rgb(0x00, 0xCC, 0xBB) // bright teal scales
	snakeTealDark  = rgb(0x00, 0x88, 0x77) // darker teal for shading
	snakeTealDeep  = rgb(0x00, 0x55, 0x4A) // deep shadow teal
	snakeGreen     = rgb(0x33, 0xDD, 0x77) // green accent scales
	snakeGreenDark = rgb(0x22, 0x99, 0x55) // darker green
	snakeOrange    = rgb(0xEE, 0x88, 0x22) // orange/copper ridge scales
	snakeOrangeLt  = rgb(0xFF, 0xAA, 0x44) // lighter orange highlight
	snakeCopper    = rgb(0xCC, 0x66, 0x11) // copper accent
	snakeEyeCyan   = rgb(0x00, 0xFF, 0xFF) // bright cyan eye
	snakeEyePupil  = rgb(0x11, 0x11, 0x22) // dark pupil
	snakeBelly     = rgb(0xDD, 0xCC, 0x88) // pale belly underside

	// Robot enemy
	robotGunmetal = rgb(0x44, 0x55, 0x66)
	robotDark     = rgb(0x22, 0x33, 0x44)
	robotRedEye   = rgb(0xFF, 0x22, 0x33)
	robotRedDim   = rgb(0xBB, 0x11, 0x22)
	robotRedGlow  = rgb(0xFF, 0x66, 0x55)
	robotAccent   = rgb(0xFF, 0x44, 0x44)
	robotChrome   = rgb(0x88, 0x99, 0xAA)

	// Pickup dot
	dotGold    = rgb(0xFF, 0xCC, 0x22)
	dotAmber   = rgb(0xFF, 0x99, 0x11)
	dotYellow  = rgb(0xFF, 0xEE, 0x66)
	dotGlowDim = rgb(0xFF, 0xDD, 0x44)
	dotOrange  = rgb(0xEE, 0x77, 0x11)

	// Powerup: Magnet (hot pink/magenta)
	magnetPink = rgb(0xFF, 0x33, 0x99)
	magnetHot  = rgb(0xFF, 0x66, 0xBB)
	magnetDeep = rgb(0xCC, 0x11, 0x66)
	magnetPole = rgb(0xDD, 0xDD, 0xDD)

	// Powerup: EMP (electric blue/white)
	empBlue   = rgb(0x44, 0x88, 0xFF)
	empBright = rgb(0xAA, 0xCC, 0xFF)
	empWhite  = rgb(0xEE, 0xEE, 0xFF)
	empDeep   = rgb(0x22, 0x44, 0xBB)

	// Powerup: Shield (cyan force field)
	shieldCyan   = rgb(0x00, 0xEE, 0xEE)
	shieldBright = rgb(0x88, 0xFF, 0xFF)
	shieldDark   = rgb(0x00, 0x88, 0xAA)
	shieldCore   = rgb(0xCC, 0xFF, 0xFF)

	// Powerup: Overclock (neon green)
	overGreen  = rgb(0x33, 0xFF, 0x33)
	overBright = rgb(0xAA, 0xFF, 0x66)
	overDark   = rgb(0x11, 0xAA, 0x11)
	overYellow = rgb(0xCC, 0xFF, 0x22)

	// Powerup: Multiplier (gold/amber)
	multGold   = rgb(0xFF, 0xDD, 0x33)
	multAmber  = rgb(0xFF, 0xAA, 0x11)
	multBright = rgb(0xFF, 0xEE, 0x88)
	multDeep   = rgb(0xBB, 0x77, 0x00)

	// Tiles
	wallCyan     = rgb(0x00, 0xDD, 0xCC)
	wallDark     = rgb(0x00, 0x77, 0x66)
	wallDeep     = rgb(0x00, 0x44, 0x3B)
	wallTrace    = rgb(0x00, 0xFF, 0xEE)
	floorNavy    = rgb(0x0D, 0x11, 0x22)
	floorGrid    = rgb(0x15, 0x1E, 0x33)
	floorDot     = rgb(0x1A, 0x25, 0x3B)
	hazardRed    = rgb(0xFF, 0x22, 0x22)
	hazardYellow = rgb(0xFF, 0xDD, 0x22)
	hazardDark   = rgb(0xAA, 0x11, 0x11)
	accentLine   = rgb(0x00, 0x55, 0x55)

	// FX
	fxWhite  = rgb(0xFF, 0xFF, 0xFF)
	fxYellow = rgb(0xFF, 0xEE, 0x44)
	fxRed    = rgb(0xFF, 0x44, 0x44)
	fxGreen  = rgb(0x44, 0xFF, 0x88)
	fxCyan   = rgb(0x00, 0xFF, 0xFF)
	fxPink   = rgb(0xFF, 0x44, 0xCC)

	transparent = color.NRGBA{0, 0, 0, 0}
)

func rgb(r, g, b uint8) color.NRGBA {
	return color.NRGBA{r, g, b, 255}
}

func rgba(r, g, b, a uint8) color.NRGBA {
	return color.NRGBA{r, g, b, a}
}

// ─── Image Helpers ──────────────────────────────────────────────────────────

func newImg(w, h int) *image.NRGBA {
	return image.NewNRGBA(image.Rect(0, 0, w, h))
}

func set(img *image.NRGBA, x, y int, c color.NRGBA) {
	if x >= 0 && x < img.Bounds().Dx() && y >= 0 && y < img.Bounds().Dy() {
		img.SetNRGBA(x, y, c)
	}
}

func get(img *image.NRGBA, x, y int) color.NRGBA {
	if x >= 0 && x < img.Bounds().Dx() && y >= 0 && y < img.Bounds().Dy() {
		return img.NRGBAAt(x, y)
	}
	return transparent
}

func fill(img *image.NRGBA, c color.NRGBA) {
	b := img.Bounds()
	for y := b.Min.Y; y < b.Max.Y; y++ {
		for x := b.Min.X; x < b.Max.X; x++ {
			img.SetNRGBA(x, y, c)
		}
	}
}

func fillRect(img *image.NRGBA, x0, y0, x1, y1 int, c color.NRGBA) {
	for y := y0; y < y1; y++ {
		for x := x0; x < x1; x++ {
			set(img, x, y, c)
		}
	}
}

func drawCircle(img *image.NRGBA, cx, cy, r int, c color.NRGBA) {
	for y := cy - r; y <= cy+r; y++ {
		for x := cx - r; x <= cx+r; x++ {
			dx := float64(x - cx)
			dy := float64(y - cy)
			if dx*dx+dy*dy <= float64(r*r) {
				set(img, x, y, c)
			}
		}
	}
}

func drawCircleOutline(img *image.NRGBA, cx, cy, r int, c color.NRGBA) {
	for y := cy - r - 1; y <= cy+r+1; y++ {
		for x := cx - r - 1; x <= cx+r+1; x++ {
			dx := float64(x - cx)
			dy := float64(y - cy)
			dist := math.Sqrt(dx*dx + dy*dy)
			if dist >= float64(r)-0.5 && dist <= float64(r)+0.5 {
				set(img, x, y, c)
			}
		}
	}
}

func drawRing(img *image.NRGBA, cx, cy, rInner, rOuter int, c color.NRGBA) {
	for y := cy - rOuter - 1; y <= cy+rOuter+1; y++ {
		for x := cx - rOuter - 1; x <= cx+rOuter+1; x++ {
			dx := float64(x - cx)
			dy := float64(y - cy)
			distSq := dx*dx + dy*dy
			if distSq >= float64(rInner*rInner) && distSq <= float64(rOuter*rOuter) {
				set(img, x, y, c)
			}
		}
	}
}

func drawLine(img *image.NRGBA, x0, y0, x1, y1 int, c color.NRGBA) {
	dx := abs(x1 - x0)
	dy := abs(y1 - y0)
	sx := 1
	if x0 > x1 {
		sx = -1
	}
	sy := 1
	if y0 > y1 {
		sy = -1
	}
	err := dx - dy
	for {
		set(img, x0, y0, c)
		if x0 == x1 && y0 == y1 {
			break
		}
		e2 := 2 * err
		if e2 > -dy {
			err -= dy
			x0 += sx
		}
		if e2 < dx {
			err += dx
			y0 += sy
		}
	}
}

func abs(x int) int {
	if x < 0 {
		return -x
	}
	return x
}

func blendColor(a, b color.NRGBA, t float64) color.NRGBA {
	return color.NRGBA{
		R: uint8(float64(a.R)*(1-t) + float64(b.R)*t),
		G: uint8(float64(a.G)*(1-t) + float64(b.G)*t),
		B: uint8(float64(a.B)*(1-t) + float64(b.B)*t),
		A: uint8(float64(a.A)*(1-t) + float64(b.A)*t),
	}
}

func savePNG(img *image.NRGBA, path string) {
	dir := filepath.Dir(path)
	os.MkdirAll(dir, 0o755)
	f, err := os.Create(path)
	if err != nil {
		panic(fmt.Sprintf("create %s: %v", path, err))
	}
	defer f.Close()
	if err := png.Encode(f, img); err != nil {
		panic(fmt.Sprintf("encode %s: %v", path, err))
	}
	fmt.Printf("  wrote %s\n", filepath.Base(path))
}

// rotate90CW rotates an image 90 degrees clockwise
func rotate90CW(src *image.NRGBA) *image.NRGBA {
	b := src.Bounds()
	w, h := b.Dx(), b.Dy()
	dst := newImg(h, w)
	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			dst.SetNRGBA(h-1-y, x, src.NRGBAAt(x, y))
		}
	}
	return dst
}

// rotate180 rotates an image 180 degrees
func rotate180(src *image.NRGBA) *image.NRGBA {
	b := src.Bounds()
	w, h := b.Dx(), b.Dy()
	dst := newImg(w, h)
	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			dst.SetNRGBA(w-1-x, h-1-y, src.NRGBAAt(x, y))
		}
	}
	return dst
}

// rotate90CCW rotates an image 90 degrees counter-clockwise
func rotate90CCW(src *image.NRGBA) *image.NRGBA {
	b := src.Bounds()
	w, h := b.Dx(), b.Dy()
	dst := newImg(h, w)
	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			dst.SetNRGBA(y, w-1-x, src.NRGBAAt(x, y))
		}
	}
	return dst
}

// flipH flips an image horizontally
func flipH(src *image.NRGBA) *image.NRGBA {
	b := src.Bounds()
	w, h := b.Dx(), b.Dy()
	dst := newImg(w, h)
	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			dst.SetNRGBA(w-1-x, y, src.NRGBAAt(x, y))
		}
	}
	return dst
}

// ─── Snake Head (facing RIGHT as base) ──────────────────────────────────────
// 32x32, drawn facing right. Other directions via rotation.

func drawSnakeHeadRight(frame int) *image.NRGBA {
	img := newImg(32, 32)
	fill(img, transparent)

	// Draw scale pattern checkerboard on the head body area
	// Head shape: rounded front (right side), wider body (left side)

	// Define the head shape - a tapered shape pointing right
	for y := 0; y < 32; y++ {
		for x := 0; x < 32; x++ {
			// Head taper: wider on left, narrower toward right
			// Center at y=15.5
			cy := float64(y) - 15.5
			cx := float64(x)

			// Width at this x position
			var halfH float64
			if cx < 4 {
				// Back of head (connects to body) - full width
				halfH = 12
			} else if cx < 24 {
				// Main head body - slight taper
				halfH = 12.0 - (cx-4.0)*0.15
			} else {
				// Snout taper
				t := (cx - 24.0) / 8.0
				halfH = (12.0 - 3.0) * (1.0 - t*t)
				if halfH < 0 {
					halfH = 0
				}
			}

			if math.Abs(cy) <= halfH {
				// Determine color based on scale pattern
				// Checker pattern for scales
				scaleX := (x / 4) % 2
				scaleY := (y / 4) % 2

				var c color.NRGBA
				if (scaleX+scaleY)%2 == 0 {
					c = snakeTeal
				} else {
					c = snakeGreen
				}

				// Top and bottom edges: orange ridge scales
				if math.Abs(cy) > halfH-2.5 {
					c = snakeOrange
				} else if math.Abs(cy) > halfH-4 {
					c = snakeOrangeLt
				}

				// Center spine ridge
				if math.Abs(cy) < 2 && x < 22 {
					c = snakeCopper
				}

				// Darker shading on edges
				if math.Abs(cy) > halfH-1.2 {
					c = snakeTealDeep
				}

				// Scale grid lines (darker)
				if x%4 == 0 || y%4 == 0 {
					c = blendColor(c, snakeTealDark, 0.4)
				}

				set(img, x, y, c)
			}
		}
	}

	// Draw eye - large and prominent like a bush viper
	eyeX, eyeY := 18, 10
	if frame == 2 {
		eyeY = 11 // slight eye movement for frame 2
	}

	// Eye socket/dark ring (makes eye pop)
	drawCircle(img, eyeX, eyeY, 5, snakeTealDeep)
	// Eye glow ring
	drawCircle(img, eyeX, eyeY, 4, rgba(0x00, 0xDD, 0xDD, 255))
	// Main eye bright cyan
	drawCircle(img, eyeX, eyeY, 3, snakeEyeCyan)
	// Inner lighter ring
	drawCircle(img, eyeX, eyeY, 2, rgba(0xCC, 0xFF, 0xFF, 255))
	// Vertical slit pupil (reptilian!)
	for dy := -3; dy <= 3; dy++ {
		set(img, eyeX, eyeY+dy, snakeEyePupil)
	}
	for dy := -2; dy <= 2; dy++ {
		px := eyeX
		if frame == 2 {
			px = eyeX + 1
		}
		set(img, px, eyeY+dy, snakeEyePupil)
	}
	// Bright highlight spot on eye
	set(img, eyeX-1, eyeY-1, fxWhite)

	// Nostril
	set(img, 26, 13, snakeTealDeep)
	set(img, 27, 13, snakeTealDeep)
	set(img, 27, 14, snakeTealDeep)

	// Mouth line
	mouthY := 16
	for x := 18; x < 30; x++ {
		if x < img.Bounds().Dx() {
			// Only draw if there's already a non-transparent pixel here
			existing := get(img, x, mouthY)
			if existing.A > 0 {
				set(img, x, mouthY, snakeTealDeep)
			}
		}
	}

	// Frame 2: mouth slightly open (showing fangs hint)
	if frame == 2 {
		for x := 22; x < 28; x++ {
			existing := get(img, x, mouthY+1)
			if existing.A > 0 {
				set(img, x, mouthY+1, snakeTealDeep)
			}
		}
		// Fang hint
		existing := get(img, 25, mouthY+1)
		if existing.A > 0 {
			set(img, 25, mouthY+1, fxWhite)
			set(img, 25, mouthY+2, fxWhite)
		}
	}

	// Belly hint on bottom
	for x := 2; x < 20; x++ {
		for y := 21; y < 24; y++ {
			existing := get(img, x, y)
			if existing.A > 0 {
				set(img, x, y, blendColor(existing, snakeBelly, 0.3))
			}
		}
	}

	// Brow ridge above eye (darker, gives menacing look)
	for x := eyeX - 4; x <= eyeX+3; x++ {
		existing := get(img, x, eyeY-5)
		if existing.A > 0 {
			set(img, x, eyeY-5, snakeCopper)
		}
		existing = get(img, x, eyeY-4)
		if existing.A > 0 {
			set(img, x, eyeY-4, snakeOrange)
		}
	}

	return img
}

// ─── Snake Body Straight Horizontal ─────────────────────────────────────────

func drawSnakeBodyStraightH() *image.NRGBA {
	img := newImg(32, 32)
	fill(img, transparent)

	halfH := 12.0

	for y := 0; y < 32; y++ {
		cy := float64(y) - 15.5
		if math.Abs(cy) > halfH {
			continue
		}

		for x := 0; x < 32; x++ {
			scaleX := (x / 4) % 2
			scaleY := (y / 4) % 2

			var c color.NRGBA
			if (scaleX+scaleY)%2 == 0 {
				c = snakeTeal
			} else {
				c = snakeGreen
			}

			// Orange ridge on top and bottom
			if math.Abs(cy) > halfH-2.5 {
				c = snakeOrange
			} else if math.Abs(cy) > halfH-4 {
				c = snakeOrangeLt
			}

			// Center spine
			if math.Abs(cy) < 2 {
				c = snakeCopper
			}

			// Edge outline
			if math.Abs(cy) > halfH-1.2 {
				c = snakeTealDeep
			}

			// Scale grid
			if x%4 == 0 || y%4 == 0 {
				c = blendColor(c, snakeTealDark, 0.4)
			}

			// Belly
			if cy > 5 && cy < 9 {
				c = blendColor(c, snakeBelly, 0.25)
			}

			set(img, x, y, c)
		}
	}

	return img
}

// ─── Snake Body Turn (up-right as base) ─────────────────────────────────────
// The turn goes from bottom edge to right edge (entering from below, exiting to right)
// This is the "ur" turn: body comes from down, turns to go right

func drawSnakeBodyTurnUR() *image.NRGBA {
	img := newImg(32, 32)
	fill(img, transparent)

	halfW := 12.0
	// Turn centered at bottom-left quadrant conceptually
	// The turn follows an arc from bottom center to right center
	// Center of the arc is at bottom-right corner area

	arcCX := 0.0  // arc center X (left side)
	arcCY := 32.0 // arc center Y (bottom)

	for y := 0; y < 32; y++ {
		for x := 0; x < 32; x++ {
			fx := float64(x) + 0.5
			fy := float64(y) + 0.5

			dx := fx - arcCX
			dy := fy - arcCY
			dist := math.Sqrt(dx*dx + dy*dy)

			// The arc band
			innerR := 16.0 - halfW
			outerR := 16.0 + halfW

			if dist >= innerR && dist <= outerR {
				// Angle check - only draw in the top-right quadrant of the arc
				angle := math.Atan2(-dy, dx) // angle from center
				if angle >= 0 && angle <= math.Pi/2 {
					// Scale pattern along the arc
					arcPos := dist - innerR
					anglePos := angle * 16.0 / (math.Pi / 2)

					scaleA := (int(arcPos) / 4) % 2
					scaleB := (int(anglePos) / 4) % 2

					var c color.NRGBA
					if (scaleA+scaleB)%2 == 0 {
						c = snakeTeal
					} else {
						c = snakeGreen
					}

					// Edge ridges (orange on outer/inner edges)
					edgeDist := math.Min(dist-innerR, outerR-dist)
					if edgeDist < 2.5 {
						c = snakeOrange
					} else if edgeDist < 4 {
						c = snakeOrangeLt
					}

					// Center spine
					centerDist := math.Abs(dist - 16.0)
					if centerDist < 2 {
						c = snakeCopper
					}

					// Edge outline
					if edgeDist < 1.2 {
						c = snakeTealDeep
					}

					// Scale grid
					if int(arcPos)%4 == 0 || int(anglePos)%4 == 0 {
						c = blendColor(c, snakeTealDark, 0.4)
					}

					set(img, x, y, c)
				}
			}
		}
	}

	return img
}

// ─── Snake Tail (facing RIGHT as base) ──────────────────────────────────────

func drawSnakeTailRight(frame int) *image.NRGBA {
	img := newImg(32, 32)
	fill(img, transparent)

	for y := 0; y < 32; y++ {
		cy := float64(y) - 15.5

		for x := 0; x < 32; x++ {
			cx := float64(x)

			// Tail tapers from full width on left to point on right
			var halfH float64
			if cx < 4 {
				halfH = 12
			} else {
				// Taper
				t := (cx - 4.0) / 28.0
				halfH = 12.0 * (1.0 - t*t)
				if halfH < 0.5 {
					halfH = 0.5
				}
			}

			// Frame 2: slight wave
			if frame == 2 {
				wave := math.Sin(cx*0.3) * 1.5
				if math.Abs(cy-wave) > halfH {
					continue
				}
			} else {
				if math.Abs(cy) > halfH {
					continue
				}
			}

			scaleX := (x / 3) % 2
			scaleY := (y / 3) % 2

			var c color.NRGBA
			if (scaleX+scaleY)%2 == 0 {
				c = snakeTeal
			} else {
				c = snakeGreen
			}

			effectiveCy := cy
			if frame == 2 {
				effectiveCy = cy - math.Sin(cx*0.3)*1.5
			}

			// Orange ridge
			if math.Abs(effectiveCy) > halfH-2 {
				c = snakeOrange
			} else if math.Abs(effectiveCy) > halfH-3 {
				c = snakeOrangeLt
			}

			// Spine
			if math.Abs(effectiveCy) < 1.5 && cx < 24 {
				c = snakeCopper
			}

			// Edge
			if math.Abs(effectiveCy) > halfH-1 {
				c = snakeTealDeep
			}

			// Scale grid (smaller toward tip)
			gridSize := 3
			if x > 20 {
				gridSize = 2
			}
			if x%gridSize == 0 || y%gridSize == 0 {
				c = blendColor(c, snakeTealDark, 0.35)
			}

			// Tip highlight
			if cx > 26 && halfH > 0 {
				c = blendColor(c, snakeOrangeLt, 0.5)
			}

			set(img, x, y, c)
		}
	}

	return img
}

// ─── Robot Enemy (48x48) ────────────────────────────────────────────────────

func drawRobotEnemy(frame int) *image.NRGBA {
	img := newImg(48, 48)
	fill(img, transparent)

	// Body - main rectangle with beveled corners
	for y := 14; y < 40; y++ {
		for x := 8; x < 40; x++ {
			// Cut corners for angular look
			dx := 0
			dy := 0
			if x < 12 {
				dx = 12 - x
			} else if x >= 36 {
				dx = x - 35
			}
			if y < 18 {
				dy = 18 - y
			} else if y >= 36 {
				dy = y - 35
			}
			if dx+dy > 4 {
				continue
			}

			// Vertical gradient on body
			t := float64(y-14) / 26.0
			c := blendColor(robotGunmetal, robotDark, t*0.5)

			// Panel lines for detail
			if x == 16 || x == 31 {
				c = robotDark
			}
			if y == 27 {
				c = robotDark
			}

			set(img, x, y, c)
		}
	}

	// Chrome frame/trim
	for x := 12; x < 36; x++ {
		set(img, x, 14, robotChrome)
		set(img, x, 15, robotChrome)
		set(img, x, 38, robotChrome)
		set(img, x, 39, robotChrome)
	}
	for y := 14; y < 40; y++ {
		set(img, 10, y, robotChrome)
		set(img, 11, y, robotChrome)
		set(img, 36, y, robotChrome)
		set(img, 37, y, robotChrome)
	}

	// Neon red circuit lines on body - cross pattern
	for x := 13; x < 35; x++ {
		set(img, x, 27, robotAccent)
		set(img, x, 28, robotAccent)
	}
	for y := 16; y < 38; y++ {
		set(img, 23, y, robotAccent)
		set(img, 24, y, robotAccent)
	}
	// Circuit line intersection glow
	drawCircle(img, 23, 27, 2, robotRedGlow)

	// Red accent dots on body panels
	set(img, 15, 21, robotAccent)
	set(img, 32, 21, robotAccent)
	set(img, 15, 33, robotAccent)
	set(img, 32, 33, robotAccent)

	// Head / visor area - wider and more angular
	for y := 4; y < 15; y++ {
		for x := 10; x < 38; x++ {
			// Angular head shape
			dx := 0
			if x < 13 {
				dx = 13 - x
			} else if x >= 35 {
				dx = x - 34
			}
			dy := 0
			if y < 7 {
				dy = 7 - y
			}
			if dx+dy > 3 {
				continue
			}

			c := robotDark
			// Visor glass area - darker, wider
			if y >= 6 && y < 13 && x >= 13 && x < 35 {
				c = rgb(0x08, 0x11, 0x22)
			}
			set(img, x, y, c)
		}
	}

	// Visor frame - chrome
	for x := 12; x < 36; x++ {
		set(img, x, 5, robotChrome)
		set(img, x, 13, robotChrome)
	}
	for y := 5; y < 14; y++ {
		set(img, 12, y, robotChrome)
		set(img, 35, y, robotChrome)
	}

	// Eyes - large menacing red, very bright
	eyeR := robotRedEye
	eyeCenter := robotRedGlow
	if frame == 2 {
		eyeR = robotRedGlow
		eyeCenter = fxWhite
	}

	// Left eye - larger
	drawCircle(img, 19, 9, 3, eyeR)
	drawCircle(img, 19, 9, 2, eyeCenter)
	set(img, 19, 9, fxWhite)
	// Eye glow halo
	drawCircle(img, 19, 9, 4, rgba(0xFF, 0x33, 0x33, 0x55))

	// Right eye - larger
	drawCircle(img, 29, 9, 3, eyeR)
	drawCircle(img, 29, 9, 2, eyeCenter)
	set(img, 29, 9, fxWhite)
	drawCircle(img, 29, 9, 4, rgba(0xFF, 0x33, 0x33, 0x55))

	// Frame 2: eyes pulse brighter with bigger glow
	if frame == 2 {
		drawCircle(img, 19, 9, 5, rgba(0xFF, 0x44, 0x44, 0x44))
		drawCircle(img, 29, 9, 5, rgba(0xFF, 0x44, 0x44, 0x44))
	}

	// Antenna - centered
	antennaX := 24
	for y := 0; y < 5; y++ {
		set(img, antennaX, y, robotChrome)
		set(img, antennaX-1, y, robotChrome)
	}
	// Antenna tip - blinks between frames
	if frame == 1 {
		drawCircle(img, antennaX, 0, 1, robotRedEye)
		set(img, antennaX, 0, robotRedGlow)
	} else {
		drawCircle(img, antennaX, 0, 2, robotRedGlow)
		set(img, antennaX, 0, fxWhite)
		set(img, antennaX-1, 0, robotRedGlow)
		set(img, antennaX+1, 0, robotRedGlow)
	}

	// Legs/treads - wider, more mechanical
	for y := 40; y < 47; y++ {
		for x := 10; x < 20; x++ {
			c := robotDark
			if y%2 == 0 {
				c = robotGunmetal
			}
			// Tread ridges
			if x%3 == 0 {
				c = robotChrome
			}
			set(img, x, y, c)
		}
		for x := 28; x < 38; x++ {
			c := robotDark
			if y%2 == 0 {
				c = robotGunmetal
			}
			if x%3 == 0 {
				c = robotChrome
			}
			set(img, x, y, c)
		}
	}

	// Arm/weapon stubs on sides
	for y := 20; y < 30; y++ {
		for x := 5; x < 10; x++ {
			set(img, x, y, robotGunmetal)
		}
		for x := 38; x < 43; x++ {
			set(img, x, y, robotGunmetal)
		}
	}
	// Arm chrome caps
	for x := 5; x < 10; x++ {
		set(img, x, 20, robotChrome)
		set(img, x, 29, robotChrome)
	}
	for x := 38; x < 43; x++ {
		set(img, x, 20, robotChrome)
		set(img, x, 29, robotChrome)
	}

	return img
}

// ─── Pickup Dot ─────────────────────────────────────────────────────────────

func drawPickupDot(frame int) *image.NRGBA {
	img := newImg(32, 32)
	fill(img, transparent)

	cx, cy := 15, 15

	// Outer glow
	glowR := 11
	if frame == 2 {
		glowR = 12
	}
	for y := 0; y < 32; y++ {
		for x := 0; x < 32; x++ {
			dx := float64(x - cx)
			dy := float64(y - cy)
			dist := math.Sqrt(dx*dx + dy*dy)
			if dist <= float64(glowR) && dist > float64(glowR)-3 {
				alpha := uint8(80 * (1.0 - (float64(glowR)-dist)/3.0))
				set(img, x, y, rgba(0xFF, 0xCC, 0x22, alpha))
			}
		}
	}

	// Main orb
	orbR := 7
	if frame == 2 {
		orbR = 8
	}
	for y := 0; y < 32; y++ {
		for x := 0; x < 32; x++ {
			dx := float64(x - cx)
			dy := float64(y - cy)
			dist := math.Sqrt(dx*dx + dy*dy)
			if dist <= float64(orbR) {
				// Radial gradient
				t := dist / float64(orbR)
				c := blendColor(dotYellow, dotAmber, t)

				// Sunburst rays
				angle := math.Atan2(dy, dx)
				rayPhase := 0.0
				if frame == 2 {
					rayPhase = 0.3
				}
				ray := math.Sin(angle*8 + rayPhase)
				if ray > 0.5 {
					c = blendColor(c, dotGold, 0.3)
				}

				set(img, x, y, c)
			}
		}
	}

	// Bright center
	drawCircle(img, cx, cy, 3, dotYellow)
	drawCircle(img, cx, cy, 1, fxWhite)

	// Sparkle points
	sparkOff := 0
	if frame == 2 {
		sparkOff = 1
	}
	set(img, cx, cy-orbR-1+sparkOff, dotGold)
	set(img, cx, cy+orbR+1-sparkOff, dotGold)
	set(img, cx-orbR-1+sparkOff, cy, dotGold)
	set(img, cx+orbR+1-sparkOff, cy, dotGold)

	return img
}

// ─── Powerup: Magnet ────────────────────────────────────────────────────────

func drawPowerupMagnet(frame int) *image.NRGBA {
	img := newImg(32, 32)
	fill(img, transparent)

	// U-shaped magnet
	// Left pole
	for y := 6; y < 24; y++ {
		for x := 6; x < 12; x++ {
			if y < 10 {
				set(img, x, y, magnetPole)
			} else {
				set(img, x, y, magnetPink)
			}
		}
	}
	// Right pole
	for y := 6; y < 24; y++ {
		for x := 20; x < 26; x++ {
			if y < 10 {
				set(img, x, y, magnetPole)
			} else {
				set(img, x, y, magnetPink)
			}
		}
	}
	// Bottom curve
	for y := 20; y < 28; y++ {
		for x := 6; x < 26; x++ {
			dx := float64(x) - 16.0
			dy := float64(y) - 20.0
			// Elliptical bottom
			if dy >= 0 {
				outerCheck := (dx*dx)/(10.0*10.0) + (dy*dy)/(8.0*8.0)
				innerCheck := (dx*dx)/(4.0*4.0) + (dy*dy)/(4.0*4.0)
				if outerCheck <= 1.0 && innerCheck >= 1.0 {
					set(img, x, y, magnetPink)
				}
			}
		}
	}

	// Hot glow edges
	for y := 0; y < 32; y++ {
		for x := 0; x < 32; x++ {
			c := get(img, x, y)
			if c.A > 0 && c != magnetPole {
				// Check if edge pixel
				neighbors := 0
				for _, d := range [][2]int{{-1, 0}, {1, 0}, {0, -1}, {0, 1}} {
					nc := get(img, x+d[0], y+d[1])
					if nc.A == 0 {
						neighbors++
					}
				}
				if neighbors > 0 {
					set(img, x, y, magnetHot)
				}
			}
		}
	}

	// Magnetic field lines (animation)
	if frame == 1 {
		set(img, 4, 7, magnetHot)
		set(img, 3, 8, magnetHot)
		set(img, 27, 7, magnetHot)
		set(img, 28, 8, magnetHot)
	} else {
		set(img, 3, 6, magnetHot)
		set(img, 2, 7, magnetHot)
		set(img, 28, 6, magnetHot)
		set(img, 29, 7, magnetHot)
	}

	// Dark outline
	for y := 10; y < 24; y++ {
		for x := 6; x < 26; x++ {
			c := get(img, x, y)
			if c == magnetPink {
				// Darker shade for depth
				if y > 20 {
					set(img, x, y, magnetDeep)
				}
			}
		}
	}

	return img
}

// ─── Powerup: EMP ───────────────────────────────────────────────────────────

func drawPowerupEMP(frame int) *image.NRGBA {
	img := newImg(32, 32)
	fill(img, transparent)

	cx, cy := 15, 15

	// Electric burst background
	burstR := 12
	if frame == 2 {
		burstR = 13
	}
	for y := 0; y < 32; y++ {
		for x := 0; x < 32; x++ {
			dx := float64(x - cx)
			dy := float64(y - cy)
			dist := math.Sqrt(dx*dx + dy*dy)
			if dist <= float64(burstR) {
				t := dist / float64(burstR)
				c := blendColor(empWhite, empBlue, t)
				set(img, x, y, c)
			}
		}
	}

	// Lightning bolt shape in center
	bolt := [][2]int{
		{17, 4}, {14, 14}, {18, 13},
		{13, 26}, {19, 16}, {15, 17},
		{17, 4},
	}
	// Fill the bolt area
	for y := 4; y < 27; y++ {
		for x := 10; x < 22; x++ {
			if pointInPoly(x, y, bolt) {
				set(img, x, y, empWhite)
			}
		}
	}

	// Bolt outline
	for i := 0; i < len(bolt)-1; i++ {
		drawLine(img, bolt[i][0], bolt[i][1], bolt[i+1][0], bolt[i+1][1], empBright)
	}

	// Spark effects
	sparkOff := 0
	if frame == 2 {
		sparkOff = 2
	}
	set(img, 6+sparkOff, 8, empBright)
	set(img, 25-sparkOff, 10, empBright)
	set(img, 8, 22-sparkOff, empBright)
	set(img, 24, 20+sparkOff, empBright)

	// Center glow
	drawCircle(img, cx, cy, 3, empWhite)

	return img
}

// simple point-in-polygon for convex-ish shapes
func pointInPoly(px, py int, poly [][2]int) bool {
	n := len(poly)
	inside := false
	j := n - 1
	for i := 0; i < n; i++ {
		xi, yi := float64(poly[i][0]), float64(poly[i][1])
		xj, yj := float64(poly[j][0]), float64(poly[j][1])
		fpx, fpy := float64(px)+0.5, float64(py)+0.5

		if ((yi > fpy) != (yj > fpy)) && (fpx < (xj-xi)*(fpy-yi)/(yj-yi)+xi) {
			inside = !inside
		}
		j = i
	}
	return inside
}

// ─── Powerup: Shield ────────────────────────────────────────────────────────

func drawPowerupShield(frame int) *image.NRGBA {
	img := newImg(32, 32)
	fill(img, transparent)

	cx, cy := 15, 16

	// Shield shape - classic pointed-bottom shield
	for y := 4; y < 28; y++ {
		for x := 4; x < 28; x++ {
			fx := float64(x) - float64(cx)
			fy := float64(y)

			var halfW float64
			if fy < 8 {
				halfW = 10
			} else if fy < 20 {
				halfW = 10.0 - (fy-8.0)*0.2
			} else {
				t := (fy - 20.0) / 8.0
				halfW = 7.6 * (1.0 - t)
			}

			if math.Abs(fx) <= halfW {
				// Shell
				edgeDist := halfW - math.Abs(fx)
				var c color.NRGBA

				if edgeDist < 2 {
					c = shieldBright
				} else if edgeDist < 4 {
					c = shieldCyan
				} else {
					c = shieldDark
				}

				// Inner highlight
				if edgeDist > 6 && fy > 6 && fy < 18 {
					c = shieldCore
				}

				// Top edge
				if fy < 6 {
					c = shieldBright
				}

				set(img, x, y, c)
			}
		}
	}

	// Energy pulse effect
	pulseR := 13
	if frame == 2 {
		pulseR = 14
	}
	drawCircleOutline(img, cx, cy, pulseR, rgba(0x00, 0xFF, 0xFF, 0x66))
	if frame == 2 {
		drawCircleOutline(img, cx, cy, pulseR-1, rgba(0x00, 0xFF, 0xFF, 0x33))
	}

	return img
}

// ─── Powerup: Overclock ─────────────────────────────────────────────────────

func drawPowerupOverclock(frame int) *image.NRGBA {
	img := newImg(32, 32)
	fill(img, transparent)

	cx, cy := 15, 15

	// Circular speed indicator
	drawCircle(img, cx, cy, 12, overDark)
	drawCircleOutline(img, cx, cy, 12, overGreen)
	drawCircleOutline(img, cx, cy, 11, overGreen)

	// Inner circle
	drawCircle(img, cx, cy, 8, rgb(0x08, 0x55, 0x08))
	drawCircleOutline(img, cx, cy, 8, overGreen)

	// Lightning bolt in center
	boltPts := [][2]int{
		{16, 8}, {13, 15}, {16, 14},
		{12, 22}, {17, 15}, {15, 16},
	}
	for i := 0; i < len(boltPts)-1; i++ {
		drawLine(img, boltPts[i][0], boltPts[i][1], boltPts[i+1][0], boltPts[i+1][1], overBright)
	}

	// Speed lines
	if frame == 1 {
		drawLine(img, 2, 8, 6, 10, overGreen)
		drawLine(img, 26, 10, 30, 8, overGreen)
		drawLine(img, 2, 22, 6, 20, overGreen)
		drawLine(img, 26, 20, 30, 22, overGreen)
	} else {
		drawLine(img, 1, 10, 5, 12, overBright)
		drawLine(img, 27, 12, 31, 10, overBright)
		drawLine(img, 1, 20, 5, 18, overBright)
		drawLine(img, 27, 18, 31, 20, overBright)
	}

	// Center glow
	drawCircle(img, cx, cy, 2, overYellow)
	set(img, cx, cy, fxWhite)

	return img
}

// ─── Powerup: Multiplier Orb ────────────────────────────────────────────────

func drawPowerupMultiplier(frame int) *image.NRGBA {
	img := newImg(32, 32)
	fill(img, transparent)

	cx, cy := 15, 15

	// Outer glow
	glowR := 13
	if frame == 2 {
		glowR = 14
	}
	for y := 0; y < 32; y++ {
		for x := 0; x < 32; x++ {
			dx := float64(x - cx)
			dy := float64(y - cy)
			dist := math.Sqrt(dx*dx + dy*dy)
			if dist <= float64(glowR) && dist > float64(glowR)-3 {
				alpha := uint8(60.0 * (1.0 - (float64(glowR)-dist)/3.0))
				set(img, x, y, rgba(0xFF, 0xDD, 0x33, alpha))
			}
		}
	}

	// Main orb
	drawCircle(img, cx, cy, 10, multAmber)
	drawCircle(img, cx, cy, 8, multGold)
	drawCircle(img, cx, cy, 5, multBright)

	// "X" symbol for multiplier
	drawLine(img, cx-3, cy-3, cx+3, cy+3, multDeep)
	drawLine(img, cx-3, cy+3, cx+3, cy-3, multDeep)
	drawLine(img, cx-2, cy-3, cx+4, cy+3, multDeep)
	drawLine(img, cx-2, cy+3, cx+4, cy-3, multDeep)

	// Star points
	starOff := 0
	if frame == 2 {
		starOff = 1
	}
	// Cardinal star points
	for i := 0; i < 4; i++ {
		angle := float64(i) * math.Pi / 2
		px := cx + int(math.Cos(angle)*float64(11+starOff))
		py := cy + int(math.Sin(angle)*float64(11+starOff))
		set(img, px, py, multBright)
		set(img, px+1, py, multGold)
		set(img, px, py+1, multGold)
	}
	// Diagonal star points
	for i := 0; i < 4; i++ {
		angle := float64(i)*math.Pi/2 + math.Pi/4
		px := cx + int(math.Cos(angle)*float64(9+starOff))
		py := cy + int(math.Sin(angle)*float64(9+starOff))
		set(img, px, py, multGold)
	}

	return img
}

// ─── Tiles ──────────────────────────────────────────────────────────────────

func drawTileWall() *image.NRGBA {
	img := newImg(32, 32)

	// Base dark teal
	fill(img, wallDeep)

	// Circuit board pattern
	// Horizontal traces
	for _, ty := range []int{4, 12, 20, 28} {
		for x := 0; x < 32; x++ {
			set(img, x, ty, wallDark)
			set(img, x, ty+1, wallDark)
		}
	}

	// Vertical traces
	for _, tx := range []int{4, 12, 20, 28} {
		for y := 0; y < 32; y++ {
			set(img, tx, y, wallDark)
			set(img, tx+1, y, wallDark)
		}
	}

	// Bright trace highlights (the circuit lines)
	for _, ty := range []int{7, 15, 23} {
		for x := 2; x < 30; x++ {
			set(img, x, ty, wallCyan)
		}
	}
	for _, tx := range []int{7, 15, 23} {
		for y := 2; y < 30; y++ {
			set(img, tx, y, wallCyan)
		}
	}

	// Intersection nodes (bright dots at trace crossings)
	for _, tx := range []int{7, 15, 23} {
		for _, ty := range []int{7, 15, 23} {
			set(img, tx, ty, wallTrace)
			set(img, tx+1, ty, wallTrace)
			set(img, tx, ty+1, wallTrace)
			set(img, tx+1, ty+1, wallTrace)
		}
	}

	// Fill panels between traces with lighter teal
	panels := [][4]int{
		{1, 1, 6, 6}, {9, 1, 14, 6}, {17, 1, 22, 6}, {25, 1, 31, 6},
		{1, 9, 6, 14}, {9, 9, 14, 14}, {17, 9, 22, 14}, {25, 9, 31, 14},
		{1, 17, 6, 22}, {9, 17, 14, 22}, {17, 17, 22, 22}, {25, 17, 31, 22},
		{1, 25, 6, 31}, {9, 25, 14, 31}, {17, 25, 22, 31}, {25, 25, 31, 31},
	}
	for _, p := range panels {
		for y := p[1]; y < p[3]; y++ {
			for x := p[0]; x < p[2]; x++ {
				// Slight gradient for depth
				t := float64(y-p[1]) / float64(p[3]-p[1])
				c := blendColor(wallCyan, wallDark, t*0.5)
				set(img, x, y, c)
			}
		}
	}

	// Scanline effect (subtle)
	for y := 0; y < 32; y += 2 {
		for x := 0; x < 32; x++ {
			c := get(img, x, y)
			set(img, x, y, blendColor(c, wallDeep, 0.1))
		}
	}

	// Border/edge highlight
	for x := 0; x < 32; x++ {
		set(img, x, 0, wallDeep)
		set(img, x, 31, wallDeep)
	}
	for y := 0; y < 32; y++ {
		set(img, 0, y, wallDeep)
		set(img, 31, y, wallDeep)
	}

	return img
}

func drawTileFloor() *image.NRGBA {
	img := newImg(32, 32)

	fill(img, floorNavy)

	// Subtle grid
	for x := 0; x < 32; x++ {
		set(img, x, 0, floorGrid)
		set(img, x, 31, floorGrid)
	}
	for y := 0; y < 32; y++ {
		set(img, 0, y, floorGrid)
		set(img, 31, y, floorGrid)
	}

	// Cross grid at center
	for x := 0; x < 32; x++ {
		set(img, x, 15, floorGrid)
		set(img, x, 16, floorGrid)
	}
	for y := 0; y < 32; y++ {
		set(img, 15, y, floorGrid)
		set(img, 16, y, floorGrid)
	}

	// Corner dots
	set(img, 0, 0, floorDot)
	set(img, 31, 0, floorDot)
	set(img, 0, 31, floorDot)
	set(img, 31, 31, floorDot)

	return img
}

func drawTileHazard() *image.NRGBA {
	img := newImg(32, 32)

	// Chevron/warning stripe pattern
	for y := 0; y < 32; y++ {
		for x := 0; x < 32; x++ {
			// Diagonal stripes
			stripe := ((x + y) / 4) % 2
			if stripe == 0 {
				set(img, x, y, hazardRed)
			} else {
				set(img, x, y, hazardYellow)
			}
		}
	}

	// Darker border
	for x := 0; x < 32; x++ {
		set(img, x, 0, hazardDark)
		set(img, x, 31, hazardDark)
	}
	for y := 0; y < 32; y++ {
		set(img, 0, y, hazardDark)
		set(img, 31, y, hazardDark)
	}

	// Center warning symbol (!)
	for y := 8; y < 20; y++ {
		set(img, 15, y, rgb(0x11, 0x00, 0x00))
		set(img, 16, y, rgb(0x11, 0x00, 0x00))
	}
	set(img, 15, 23, rgb(0x11, 0x00, 0x00))
	set(img, 16, 23, rgb(0x11, 0x00, 0x00))
	set(img, 15, 24, rgb(0x11, 0x00, 0x00))
	set(img, 16, 24, rgb(0x11, 0x00, 0x00))

	return img
}

func drawTileAccentA() *image.NRGBA {
	img := newImg(32, 32)

	fill(img, floorNavy)

	// Grid like floor
	for x := 0; x < 32; x++ {
		set(img, x, 0, floorGrid)
		set(img, x, 31, floorGrid)
	}
	for y := 0; y < 32; y++ {
		set(img, 0, y, floorGrid)
		set(img, 31, y, floorGrid)
	}

	// Corner accent - top-left glowing corner
	for y := 0; y < 8; y++ {
		for x := 0; x < 8; x++ {
			dist := float64(x+y) / 14.0
			if dist < 1.0 {
				c := blendColor(accentLine, floorNavy, dist)
				set(img, x, y, c)
			}
		}
	}

	// Subtle grid cross
	for x := 0; x < 32; x++ {
		set(img, x, 15, floorGrid)
		set(img, x, 16, floorGrid)
	}
	for y := 0; y < 32; y++ {
		set(img, 15, y, floorGrid)
		set(img, 16, y, floorGrid)
	}

	return img
}

func drawTileAccentB() *image.NRGBA {
	img := newImg(32, 32)

	fill(img, floorNavy)

	// Grid
	for x := 0; x < 32; x++ {
		set(img, x, 0, floorGrid)
		set(img, x, 31, floorGrid)
	}
	for y := 0; y < 32; y++ {
		set(img, 0, y, floorGrid)
		set(img, 31, y, floorGrid)
	}

	// Bottom-right accent
	for y := 24; y < 32; y++ {
		for x := 24; x < 32; x++ {
			dist := float64((31-x)+(31-y)) / 14.0
			if dist < 1.0 {
				c := blendColor(accentLine, floorNavy, dist)
				set(img, x, y, c)
			}
		}
	}

	// Subtle grid cross
	for x := 0; x < 32; x++ {
		set(img, x, 15, floorGrid)
		set(img, x, 16, floorGrid)
	}
	for y := 0; y < 32; y++ {
		set(img, 15, y, floorGrid)
		set(img, 16, y, floorGrid)
	}

	return img
}

// ─── FX Icons ───────────────────────────────────────────────────────────────

func drawFXHitFlash() *image.NRGBA {
	img := newImg(32, 32)
	fill(img, transparent)

	cx, cy := 15, 15

	// Starburst
	for i := 0; i < 8; i++ {
		angle := float64(i) * math.Pi / 4.0
		for d := 0.0; d < 14.0; d += 0.5 {
			x := cx + int(math.Cos(angle)*d)
			y := cy + int(math.Sin(angle)*d)
			t := d / 14.0
			c := blendColor(fxWhite, fxYellow, t)
			if d > 10 {
				c = rgba(c.R, c.G, c.B, uint8(255*(14-d)/4))
			}
			set(img, x, y, c)
		}
	}

	// Center
	drawCircle(img, cx, cy, 4, fxWhite)
	drawCircle(img, cx, cy, 2, fxYellow)

	return img
}

func drawIconCombo() *image.NRGBA {
	img := newImg(32, 32)
	fill(img, transparent)

	// Chain link circles
	drawCircleOutline(img, 11, 15, 7, fxYellow)
	drawCircleOutline(img, 11, 15, 6, fxYellow)
	drawCircleOutline(img, 21, 15, 7, fxYellow)
	drawCircleOutline(img, 21, 15, 6, fxYellow)

	// Sparkle at intersection
	set(img, 16, 15, fxWhite)
	set(img, 15, 14, fxWhite)
	set(img, 17, 16, fxWhite)

	return img
}

func drawIconRiskBonus() *image.NRGBA {
	img := newImg(32, 32)
	fill(img, transparent)

	// Exclamation mark in red
	for y := 4; y < 20; y++ {
		fillRect(img, 14, y, 18, y+1, fxRed)
	}
	fillRect(img, 14, 23, 18, 27, fxRed)

	// Border glow
	for y := 0; y < 32; y++ {
		for x := 0; x < 32; x++ {
			c := get(img, x, y)
			if c == fxRed {
				for _, d := range [][2]int{{-1, 0}, {1, 0}, {0, -1}, {0, 1}} {
					nc := get(img, x+d[0], y+d[1])
					if nc.A == 0 {
						set(img, x+d[0], y+d[1], rgba(0xFF, 0x44, 0x44, 0x88))
					}
				}
			}
		}
	}

	return img
}

func drawIconCloseCall() *image.NRGBA {
	img := newImg(32, 32)
	fill(img, transparent)

	// Two circles nearly touching (close call)
	drawCircle(img, 10, 15, 6, fxCyan)
	drawCircle(img, 10, 15, 4, rgb(0x00, 0xAA, 0xAA))

	drawCircle(img, 22, 15, 6, fxRed)
	drawCircle(img, 22, 15, 4, rgb(0xAA, 0x22, 0x22))

	// Spark between them
	set(img, 16, 14, fxWhite)
	set(img, 16, 15, fxYellow)
	set(img, 16, 16, fxWhite)

	return img
}

func drawIconCleanPath() *image.NRGBA {
	img := newImg(32, 32)
	fill(img, transparent)

	// Winding path with sparkles
	points := [][2]int{
		{4, 26}, {8, 22}, {12, 24}, {16, 18},
		{20, 14}, {24, 10}, {28, 6},
	}
	for i := 0; i < len(points)-1; i++ {
		drawLine(img, points[i][0], points[i][1], points[i+1][0], points[i+1][1], fxGreen)
		drawLine(img, points[i][0], points[i][1]+1, points[i+1][0], points[i+1][1]+1, fxGreen)
	}

	// Sparkles along path
	for i, p := range points {
		if i%2 == 0 {
			set(img, p[0], p[1]-2, fxWhite)
			set(img, p[0]+1, p[1]-1, fxWhite)
		}
	}

	return img
}

func drawIconPerfectClear() *image.NRGBA {
	img := newImg(32, 32)
	fill(img, transparent)

	cx, cy := 15, 15

	// Star shape
	for i := 0; i < 5; i++ {
		angle := float64(i)*2*math.Pi/5 - math.Pi/2
		nextAngle := float64(i)*2*math.Pi/5 + math.Pi/5 - math.Pi/2

		// Outer point
		ox := cx + int(math.Cos(angle)*12)
		oy := cy + int(math.Sin(angle)*12)
		// Inner point
		ix := cx + int(math.Cos(nextAngle)*5)
		iy := cy + int(math.Sin(nextAngle)*5)

		drawLine(img, cx, cy, ox, oy, fxYellow)
		drawLine(img, ox, oy, ix, iy, fxYellow)

		// Thicker
		drawLine(img, cx+1, cy, ox+1, oy, fxYellow)
		drawLine(img, ox, oy+1, ix, iy+1, fxYellow)
	}

	// Fill center
	drawCircle(img, cx, cy, 5, fxYellow)
	drawCircle(img, cx, cy, 3, fxWhite)

	// Sparkle tips
	for i := 0; i < 5; i++ {
		angle := float64(i)*2*math.Pi/5 - math.Pi/2
		ox := cx + int(math.Cos(angle)*13)
		oy := cy + int(math.Sin(angle)*13)
		set(img, ox, oy, fxWhite)
	}

	return img
}

// ─── Main ───────────────────────────────────────────────────────────────────

func main() {
	base := os.Args[1] // assets directory path

	fmt.Println("=== Generating Player Snake Sprites ===")

	// Head: draw right-facing, then rotate for other directions
	for _, frame := range []int{1, 2} {
		headR := drawSnakeHeadRight(frame)
		suffix := fmt.Sprintf("f%d", frame)

		savePNG(headR, filepath.Join(base, "sprites/player", "snakeman_head_right_"+suffix+".png"))
		savePNG(rotate90CW(headR), filepath.Join(base, "sprites/player", "snakeman_head_down_"+suffix+".png"))
		savePNG(rotate180(headR), filepath.Join(base, "sprites/player", "snakeman_head_left_"+suffix+".png"))
		savePNG(rotate90CCW(headR), filepath.Join(base, "sprites/player", "snakeman_head_up_"+suffix+".png"))
	}

	// Body straight
	bodyH := drawSnakeBodyStraightH()
	savePNG(bodyH, filepath.Join(base, "sprites/player", "snakeman_body_straight_h.png"))
	savePNG(rotate90CW(bodyH), filepath.Join(base, "sprites/player", "snakeman_body_straight_v.png"))

	// Body turns: draw UR (up-right), then rotate/flip for others
	turnUR := drawSnakeBodyTurnUR()
	savePNG(turnUR, filepath.Join(base, "sprites/player", "snakeman_body_turn_ur.png"))
	// RD = rotate UR 90 CW
	savePNG(rotate90CW(turnUR), filepath.Join(base, "sprites/player", "snakeman_body_turn_rd.png"))
	// DL = rotate UR 180
	savePNG(rotate180(turnUR), filepath.Join(base, "sprites/player", "snakeman_body_turn_dl.png"))
	// LU = rotate UR 90 CCW
	savePNG(rotate90CCW(turnUR), filepath.Join(base, "sprites/player", "snakeman_body_turn_lu.png"))

	// Tail: draw right-facing, then rotate
	for _, frame := range []int{1, 2} {
		tailR := drawSnakeTailRight(frame)
		suffix := fmt.Sprintf("f%d", frame)

		savePNG(tailR, filepath.Join(base, "sprites/player", "snakeman_tail_right_"+suffix+".png"))
		savePNG(rotate90CW(tailR), filepath.Join(base, "sprites/player", "snakeman_tail_down_"+suffix+".png"))
		savePNG(rotate180(tailR), filepath.Join(base, "sprites/player", "snakeman_tail_left_"+suffix+".png"))
		savePNG(rotate90CCW(tailR), filepath.Join(base, "sprites/player", "snakeman_tail_up_"+suffix+".png"))
	}

	fmt.Println("\n=== Generating Robot Enemy Sprites ===")
	savePNG(drawRobotEnemy(1), filepath.Join(base, "sprites", "robot_enemy_f1.png"))
	savePNG(drawRobotEnemy(2), filepath.Join(base, "sprites", "robot_enemy_f2.png"))

	fmt.Println("\n=== Generating Pickup Dot Sprites ===")
	savePNG(drawPickupDot(1), filepath.Join(base, "sprites", "pickup_dot_f1.png"))
	savePNG(drawPickupDot(2), filepath.Join(base, "sprites", "pickup_dot_f2.png"))

	fmt.Println("\n=== Generating Powerup Sprites ===")
	savePNG(drawPowerupMagnet(1), filepath.Join(base, "sprites", "powerup_magnet_f1.png"))
	savePNG(drawPowerupMagnet(2), filepath.Join(base, "sprites", "powerup_magnet_f2.png"))
	savePNG(drawPowerupEMP(1), filepath.Join(base, "sprites", "powerup_emp_f1.png"))
	savePNG(drawPowerupEMP(2), filepath.Join(base, "sprites", "powerup_emp_f2.png"))
	savePNG(drawPowerupShield(1), filepath.Join(base, "sprites", "powerup_shield_f1.png"))
	savePNG(drawPowerupShield(2), filepath.Join(base, "sprites", "powerup_shield_f2.png"))
	savePNG(drawPowerupOverclock(1), filepath.Join(base, "sprites", "powerup_overclock_f1.png"))
	savePNG(drawPowerupOverclock(2), filepath.Join(base, "sprites", "powerup_overclock_f2.png"))
	savePNG(drawPowerupMultiplier(1), filepath.Join(base, "sprites", "powerup_multiplier_orb_f1.png"))
	savePNG(drawPowerupMultiplier(2), filepath.Join(base, "sprites", "powerup_multiplier_orb_f2.png"))

	fmt.Println("\n=== Generating Tile Sprites ===")
	savePNG(drawTileWall(), filepath.Join(base, "tiles", "tile_wall.png"))
	savePNG(drawTileFloor(), filepath.Join(base, "tiles", "tile_floor.png"))
	savePNG(drawTileHazard(), filepath.Join(base, "tiles", "tile_hazard.png"))
	savePNG(drawTileAccentA(), filepath.Join(base, "tiles", "tile_quadrant_accent_a.png"))
	savePNG(drawTileAccentB(), filepath.Join(base, "tiles", "tile_quadrant_accent_b.png"))

	fmt.Println("\n=== Generating FX Icons ===")
	savePNG(drawFXHitFlash(), filepath.Join(base, "fx", "fx_hit_flash.png"))
	savePNG(drawIconCombo(), filepath.Join(base, "fx", "icon_combo.png"))
	savePNG(drawIconRiskBonus(), filepath.Join(base, "fx", "icon_risk_bonus.png"))
	savePNG(drawIconCloseCall(), filepath.Join(base, "fx", "icon_close_call.png"))
	savePNG(drawIconCleanPath(), filepath.Join(base, "fx", "icon_clean_path.png"))
	savePNG(drawIconPerfectClear(), filepath.Join(base, "fx", "icon_perfect_clear.png"))

	fmt.Println("\n=== Done! All sprites generated. ===")
}
