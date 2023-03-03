<?php

/** @generate-class-entries */

/**
 * @var int
 * @cvalue PHP_IMG_AVIF
 */
const IMG_AVIF = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_IMG_GIF
 */
const IMG_GIF = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_IMG_JPG
 */
const IMG_JPG = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_IMG_JPEG
 */
const IMG_JPEG = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_IMG_PNG
 */
const IMG_PNG = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_IMG_WBMP
 */
const IMG_WBMP = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_IMG_XPM
 */
const IMG_XPM = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_IMG_WEBP
 */
const IMG_WEBP = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_IMG_BMP
 */
const IMG_BMP = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_IMG_TGA
 */
const IMG_TGA = UNKNOWN;

/* constant for webp encoding */

#ifdef gdWebpLossless
/**
 * @var int
 * @cvalue gdWebpLossless
 */
const IMG_WEBP_LOSSLESS = UNKNOWN;
#endif

/* special colours for gd */

/**
 * @var int
 * @cvalue gdTiled
 */
const IMG_COLOR_TILED = UNKNOWN;
/**
 * @var int
 * @cvalue gdStyled
 */
const IMG_COLOR_STYLED = UNKNOWN;
/**
 * @var int
 * @cvalue gdBrushed
 */
const IMG_COLOR_BRUSHED = UNKNOWN;
/**
 * @var int
 * @cvalue gdStyledBrushed
 */
const IMG_COLOR_STYLEDBRUSHED = UNKNOWN;
/**
 * @var int
 * @cvalue gdTransparent
 */
const IMG_COLOR_TRANSPARENT = UNKNOWN;

/* for imagefilledarc */

/**
 * @var int
 * @cvalue gdArc
 */
const IMG_ARC_ROUNDED = UNKNOWN;
/**
 * @var int
 * @cvalue gdPie
 */
const IMG_ARC_PIE = UNKNOWN;
/**
 * @var int
 * @cvalue gdChord
 */
const IMG_ARC_CHORD = UNKNOWN;
/**
 * @var int
 * @cvalue gdNoFill
 */
const IMG_ARC_NOFILL = UNKNOWN;
/**
 * @var int
 * @cvalue gdEdged
 */
const IMG_ARC_EDGED = UNKNOWN;

/* GD2 image format types */

/**
 * @var int
 * @cvalue GD2_FMT_RAW
 */
const IMG_GD2_RAW = UNKNOWN;
/**
 * @var int
 * @cvalue GD2_FMT_COMPRESSED
 */
const IMG_GD2_COMPRESSED = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_GD_FLIP_HORIZONTAL
 */
const IMG_FLIP_HORIZONTAL = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_GD_FLIP_VERTICAL
 */
const IMG_FLIP_VERTICAL = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_GD_FLIP_BOTH
 */
const IMG_FLIP_BOTH = UNKNOWN;
/**
 * @var int
 * @cvalue gdEffectReplace
 */
const IMG_EFFECT_REPLACE = UNKNOWN;
/**
 * @var int
 * @cvalue gdEffectAlphaBlend
 */
const IMG_EFFECT_ALPHABLEND = UNKNOWN;
/**
 * @var int
 * @cvalue gdEffectNormal
 */
const IMG_EFFECT_NORMAL = UNKNOWN;
/**
 * @var int
 * @cvalue gdEffectOverlay
 */
const IMG_EFFECT_OVERLAY = UNKNOWN;

#ifdef gdEffectMultiply
/**
 * @var int
 * @cvalue gdEffectMultiply
 */
const IMG_EFFECT_MULTIPLY = UNKNOWN;
#endif

/**
 * @var int
 * @cvalue GD_CROP_DEFAULT
 */
const IMG_CROP_DEFAULT = UNKNOWN;
/**
 * @var int
 * @cvalue GD_CROP_TRANSPARENT
 */
const IMG_CROP_TRANSPARENT = UNKNOWN;
/**
 * @var int
 * @cvalue GD_CROP_BLACK
 */
const IMG_CROP_BLACK = UNKNOWN;
/**
 * @var int
 * @cvalue GD_CROP_WHITE
 */
const IMG_CROP_WHITE = UNKNOWN;
/**
 * @var int
 * @cvalue GD_CROP_SIDES
 */
const IMG_CROP_SIDES = UNKNOWN;
/**
 * @var int
 * @cvalue GD_CROP_THRESHOLD
 */
const IMG_CROP_THRESHOLD = UNKNOWN;

/**
 * @var int
 * @cvalue GD_BELL
 */
const IMG_BELL = UNKNOWN;
/**
 * @var int
 * @cvalue GD_BESSEL
 */
const IMG_BESSEL = UNKNOWN;
/**
 * @var int
 * @cvalue GD_BILINEAR_FIXED
 */
const IMG_BILINEAR_FIXED = UNKNOWN;
/**
 * @var int
 * @cvalue GD_BICUBIC
 */
const IMG_BICUBIC = UNKNOWN;
/**
 * @var int
 * @cvalue GD_BICUBIC_FIXED
 */
const IMG_BICUBIC_FIXED = UNKNOWN;
/**
 * @var int
 * @cvalue GD_BLACKMAN
 */
const IMG_BLACKMAN = UNKNOWN;
/**
 * @var int
 * @cvalue GD_BOX
 */
const IMG_BOX = UNKNOWN;
/**
 * @var int
 * @cvalue GD_BSPLINE
 */
const IMG_BSPLINE = UNKNOWN;
/**
 * @var int
 * @cvalue GD_CATMULLROM
 */
const IMG_CATMULLROM = UNKNOWN;
/**
 * @var int
 * @cvalue GD_GAUSSIAN
 */
const IMG_GAUSSIAN = UNKNOWN;
/**
 * @var int
 * @cvalue GD_GENERALIZED_CUBIC
 */
const IMG_GENERALIZED_CUBIC = UNKNOWN;
/**
 * @var int
 * @cvalue GD_HERMITE
 */
const IMG_HERMITE = UNKNOWN;
/**
 * @var int
 * @cvalue GD_HAMMING
 */
const IMG_HAMMING = UNKNOWN;
/**
 * @var int
 * @cvalue GD_HANNING
 */
const IMG_HANNING = UNKNOWN;
/**
 * @var int
 * @cvalue GD_MITCHELL
 */
const IMG_MITCHELL = UNKNOWN;
/**
 * @var int
 * @cvalue GD_POWER
 */
const IMG_POWER = UNKNOWN;
/**
 * @var int
 * @cvalue GD_QUADRATIC
 */
const IMG_QUADRATIC = UNKNOWN;
/**
 * @var int
 * @cvalue GD_SINC
 */
const IMG_SINC = UNKNOWN;
/**
 * @var int
 * @cvalue GD_NEAREST_NEIGHBOUR
 */
const IMG_NEAREST_NEIGHBOUR = UNKNOWN;
/**
 * @var int
 * @cvalue GD_WEIGHTED4
 */
const IMG_WEIGHTED4 = UNKNOWN;
/**
 * @var int
 * @cvalue GD_TRIANGLE
 */
const IMG_TRIANGLE = UNKNOWN;

/**
 * @var int
 * @cvalue GD_AFFINE_TRANSLATE
 */
const IMG_AFFINE_TRANSLATE = UNKNOWN;
/**
 * @var int
 * @cvalue GD_AFFINE_SCALE
 */
const IMG_AFFINE_SCALE = UNKNOWN;
/**
 * @var int
 * @cvalue GD_AFFINE_ROTATE
 */
const IMG_AFFINE_ROTATE = UNKNOWN;
/**
 * @var int
 * @cvalue GD_AFFINE_SHEAR_HORIZONTAL
 */
const IMG_AFFINE_SHEAR_HORIZONTAL = UNKNOWN;
/**
 * @var int
 * @cvalue GD_AFFINE_SHEAR_VERTICAL
 */
const IMG_AFFINE_SHEAR_VERTICAL = UNKNOWN;

/**
 * @var int
 * @cvalue GD_BUNDLED
 */
const GD_BUNDLED = UNKNOWN;

/* Section Filters */

/**
 * @var int
 * @cvalue IMAGE_FILTER_NEGATE
 */
const IMG_FILTER_NEGATE = UNKNOWN;
/**
 * @var int
 * @cvalue IMAGE_FILTER_GRAYSCALE
 */
const IMG_FILTER_GRAYSCALE = UNKNOWN;
/**
 * @var int
 * @cvalue IMAGE_FILTER_BRIGHTNESS
 */
const IMG_FILTER_BRIGHTNESS = UNKNOWN;
/**
 * @var int
 * @cvalue IMAGE_FILTER_CONTRAST
 */
const IMG_FILTER_CONTRAST = UNKNOWN;
/**
 * @var int
 * @cvalue IMAGE_FILTER_COLORIZE
 */
const IMG_FILTER_COLORIZE = UNKNOWN;
/**
 * @var int
 * @cvalue IMAGE_FILTER_EDGEDETECT
 */
const IMG_FILTER_EDGEDETECT = UNKNOWN;
/**
 * @var int
 * @cvalue IMAGE_FILTER_GAUSSIAN_BLUR
 */
const IMG_FILTER_GAUSSIAN_BLUR = UNKNOWN;
/**
 * @var int
 * @cvalue IMAGE_FILTER_SELECTIVE_BLUR
 */
const IMG_FILTER_SELECTIVE_BLUR = UNKNOWN;
/**
 * @var int
 * @cvalue IMAGE_FILTER_EMBOSS
 */
const IMG_FILTER_EMBOSS = UNKNOWN;
/**
 * @var int
 * @cvalue IMAGE_FILTER_MEAN_REMOVAL
 */
const IMG_FILTER_MEAN_REMOVAL = UNKNOWN;
/**
 * @var int
 * @cvalue IMAGE_FILTER_SMOOTH
 */
const IMG_FILTER_SMOOTH = UNKNOWN;
/**
 * @var int
 * @cvalue IMAGE_FILTER_PIXELATE
 */
const IMG_FILTER_PIXELATE = UNKNOWN;
/**
 * @var int
 * @cvalue IMAGE_FILTER_SCATTER
 */
const IMG_FILTER_SCATTER = UNKNOWN;

#ifdef GD_VERSION_STRING
/**
 * @var string
 * @cvalue GD_VERSION_STRING
 */
const GD_VERSION = UNKNOWN;
#endif

#if (defined(GD_MAJOR_VERSION) && defined(GD_MINOR_VERSION) && defined(GD_RELEASE_VERSION) && defined(GD_EXTRA_VERSION))
/**
 * @var int
 * @cvalue GD_MAJOR_VERSION
 */
const GD_MAJOR_VERSION = UNKNOWN;
/**
 * @var int
 * @cvalue GD_MINOR_VERSION
 */
const GD_MINOR_VERSION = UNKNOWN;
/**
 * @var int
 * @cvalue GD_RELEASE_VERSION
 */
const GD_RELEASE_VERSION = UNKNOWN;
/**
 * @var string
 * @cvalue GD_EXTRA_VERSION
 */
const GD_EXTRA_VERSION = UNKNOWN;
#endif

#ifdef HAVE_GD_PNG
/*
 * cannot include #include "png.h"
 * /usr/include/pngconf.h:310:2: error: #error png.h already includes setjmp.h with some additional fixup.
 * as error, use the values for now...
 */
/** @var int */
const PNG_NO_FILTER = 0x00;
/** @var int */
const PNG_FILTER_NONE = 0x08;
/** @var int */
const PNG_FILTER_SUB = 0x10;
/** @var int */
const PNG_FILTER_UP = 0x20;
/** @var int */
const PNG_FILTER_AVG = 0x40;
/** @var int */
const PNG_FILTER_PAETH = 0x80;
/** @var int */
const PNG_ALL_FILTERS = 0x08 | 0x10 | 0x20 | 0x40 | 0x80;
#endif

/**
 * @strict-properties
 * @not-serializable
 */
final class GdImage {}

/**
 * @strict-properties
 * @not-serializable
 */
final class GdFont {}

/**
 * @return array<string, string|bool>
 * @refcount 1
 */
function gd_info(): array {}

function imageloadfont(string $filename): GdFont|false {}

function imagesetstyle(GdImage $image, array $style): bool {}

/** @refcount 1 */
function imagecreatetruecolor(int $width, int $height): GdImage|false {}

function imageistruecolor(GdImage $image): bool {}

function imagetruecolortopalette(GdImage $image, bool $dither, int $num_colors): bool {}

function imagepalettetotruecolor(GdImage $image): bool {}

function imagecolormatch(GdImage $image1, GdImage $image2): bool {}

function imagesetthickness(GdImage $image, int $thickness): bool {}

function imagefilledellipse(GdImage $image, int $center_x, int $center_y, int $width, int $height, int $color): bool {}

function imagefilledarc(GdImage $image, int $center_x, int $center_y, int $width, int $height, int $start_angle, int $end_angle, int $color, int $style): bool {}

function imagealphablending(GdImage $image, bool $enable): bool {}

function imagesavealpha(GdImage $image, bool $enable): bool {}

function imagelayereffect(GdImage $image, int $effect): bool {}

function imagecolorallocatealpha(GdImage $image, int $red, int $green, int $blue, int $alpha): int|false {}

function imagecolorresolvealpha(GdImage $image, int $red, int $green, int $blue, int $alpha): int {}

function imagecolorclosestalpha(GdImage $image, int $red, int $green, int $blue, int $alpha): int {}

function imagecolorexactalpha(GdImage $image, int $red, int $green, int $blue, int $alpha): int {}

function imagecopyresampled(GdImage $dst_image, GdImage $src_image, int $dst_x, int $dst_y, int $src_x, int $src_y, int $dst_width, int $dst_height, int $src_width, int $src_height): bool {}

#ifdef PHP_WIN32

/** @refcount 1 */
function imagegrabwindow(int $handle, bool $client_area = false): GdImage|false {}

/** @refcount 1 */
function imagegrabscreen(): GdImage|false {}

#endif

// TODO: $ignore_transparent is ignored???
/** @refcount 1 */
function imagerotate(GdImage $image, float $angle, int $background_color, bool $ignore_transparent = false): GdImage|false {}

function imagesettile(GdImage $image, GdImage $tile): bool {}

function imagesetbrush(GdImage $image, GdImage $brush): bool {}

/** @refcount 1 */
function imagecreate(int $width, int $height): GdImage|false {}

/** @compile-time-eval */
function imagetypes(): int {}

/** @refcount 1 */
function imagecreatefromstring(string $data): GdImage|false {}

#ifdef HAVE_GD_AVIF
/** @refcount 1 */
function imagecreatefromavif(string $filename): GdImage|false {}
#endif

/** @refcount 1 */
function imagecreatefromgif(string $filename): GdImage|false {}

#ifdef HAVE_GD_JPG
/** @refcount 1 */
function imagecreatefromjpeg(string $filename): GdImage|false {}
#endif

#ifdef HAVE_GD_PNG
/** @refcount 1 */
function imagecreatefrompng(string $filename): GdImage|false {}
#endif

#ifdef HAVE_GD_WEBP
/** @refcount 1 */
function imagecreatefromwebp(string $filename): GdImage|false {}
#endif

/** @refcount 1 */
function imagecreatefromxbm(string $filename): GdImage|false {}

#ifdef HAVE_GD_XPM
/** @refcount 1 */
function imagecreatefromxpm(string $filename): GdImage|false {}
#endif

/** @refcount 1 */
function imagecreatefromwbmp(string $filename): GdImage|false {}

/** @refcount 1 */
function imagecreatefromgd(string $filename): GdImage|false {}

/** @refcount 1 */
function imagecreatefromgd2(string $filename): GdImage|false {}

/** @refcount 1 */
function imagecreatefromgd2part(string $filename, int $x, int $y, int $width, int $height): GdImage|false {}

#ifdef HAVE_GD_BMP
/** @refcount 1 */
function imagecreatefrombmp(string $filename): GdImage|false {}
#endif

#ifdef HAVE_GD_TGA
function imagecreatefromtga(string $filename): GdImage|false {}
#endif

function imagexbm(GdImage $image, ?string $filename, ?int $foreground_color = null): bool {}

#ifdef HAVE_GD_AVIF
/** @param resource|string|null $file */
function imageavif(GdImage $image, $file = null, int $quality = -1, int $speed = -1): bool {}
#endif

/** @param resource|string|null $file */
function imagegif(GdImage $image, $file = null): bool {}

#ifdef HAVE_GD_PNG
/** @param resource|string|null $file */
function imagepng(GdImage $image, $file = null, int $quality = -1, int $filters = -1): bool {}
#endif

#ifdef HAVE_GD_WEBP
/** @param resource|string|null $file */
function imagewebp(GdImage $image, $file = null, int $quality = -1): bool {}
#endif

#ifdef HAVE_GD_JPG
/** @param resource|string|null $file */
function imagejpeg(GdImage $image, $file = null, int $quality = -1): bool {}
#endif

/** @param resource|string|null $file */
function imagewbmp(GdImage $image, $file = null, ?int $foreground_color = null): bool {}

function imagegd(GdImage $image, ?string $file = null): bool {}

function imagegd2(GdImage $image, ?string $file = null, int $chunk_size = UNKNOWN, int $mode = UNKNOWN): bool {}

#ifdef HAVE_GD_BMP
/** @param resource|string|null $file */
function imagebmp(GdImage $image, $file = null, bool $compressed = true): bool {}
#endif

function imagedestroy(GdImage $image): bool {}

function imagecolorallocate(GdImage $image, int $red, int $green, int $blue): int|false {}

function imagepalettecopy(GdImage $dst, GdImage $src): void {}

function imagecolorat(GdImage $image, int $x, int $y): int|false {}

function imagecolorclosest(GdImage $image, int $red, int $green, int $blue): int {}

function imagecolorclosesthwb(GdImage $image, int $red, int $green, int $blue): int {}

function imagecolordeallocate(GdImage $image, int $color): bool {}

function imagecolorresolve(GdImage $image, int $red, int $green, int $blue): int {}

function imagecolorexact(GdImage $image, int $red, int $green, int $blue): int {}

function imagecolorset(GdImage $image, int $color, int $red, int $green, int $blue, int $alpha = 0): false|null {}

/**
 * @return array<string, int>
 * @refcount 1
 */
function imagecolorsforindex(GdImage $image, int $color): array {}

function imagegammacorrect(GdImage $image, float $input_gamma, float $output_gamma): bool {}

function imagesetpixel(GdImage $image, int $x, int $y, int $color): bool {}

function imageline(GdImage $image, int $x1, int $y1, int $x2, int $y2, int $color): bool {}

function imagedashedline(GdImage $image, int $x1, int $y1, int $x2, int $y2, int $color): bool {}

function imagerectangle(GdImage $image, int $x1, int $y1, int $x2, int $y2, int $color): bool {}

function imagefilledrectangle(GdImage $image, int $x1, int $y1, int $x2, int $y2, int $color): bool {}

function imagearc(GdImage $image, int $center_x, int $center_y, int $width, int $height, int $start_angle, int $end_angle, int $color): bool {}

function imageellipse(GdImage $image, int $center_x, int $center_y, int $width, int $height, int $color): bool {}

function imagefilltoborder(GdImage $image, int $x, int $y, int $border_color, int $color): bool {}

function imagefill(GdImage $image, int $x, int $y, int $color): bool {}

function imagecolorstotal(GdImage $image): int {}

function imagecolortransparent(GdImage $image, ?int $color = null): int {}

function imageinterlace(GdImage $image, ?bool $enable = null): bool {}

function imagepolygon(GdImage $image, array $points, int $num_points_or_color, ?int $color = null): bool {}

function imageopenpolygon(GdImage $image, array $points, int $num_points_or_color, ?int $color = null): bool {}

function imagefilledpolygon(GdImage $image, array $points, int $num_points_or_color, ?int $color = null): bool {}

function imagefontwidth(GdFont|int $font): int {}

function imagefontheight(GdFont|int $font): int {}

function imagechar(GdImage $image, GdFont|int $font, int $x, int $y, string $char, int $color): bool {}

function imagecharup(GdImage $image, GdFont|int $font, int $x, int $y, string $char, int $color): bool {}

function imagestring(GdImage $image, GdFont|int $font, int $x, int $y, string $string, int $color): bool {}

function imagestringup(GdImage $image, GdFont|int $font, int $x, int $y, string $string, int $color): bool {}

function imagecopy(GdImage $dst_image, GdImage $src_image, int $dst_x, int $dst_y, int $src_x, int $src_y, int $src_width, int $src_height): bool {}

function imagecopymerge(GdImage $dst_image, GdImage $src_image, int $dst_x, int $dst_y, int $src_x, int $src_y, int $src_width, int $src_height, int $pct): bool {}

function imagecopymergegray(GdImage $dst_image, GdImage $src_image, int $dst_x, int $dst_y, int $src_x, int $src_y, int $src_width, int $src_height, int $pct): bool {}

function imagecopyresized(GdImage $dst_image, GdImage $src_image, int $dst_x, int $dst_y, int $src_x, int $src_y, int $dst_width, int $dst_height, int $src_width, int $src_height): bool {}

function imagesx(GdImage $image): int {}

function imagesy(GdImage $image): int {}

function imagesetclip(GdImage $image, int $x1, int $y1, int $x2, int $y2): bool {}

/**
 * @return array<int, int>
 * @refcount 1
 */
function imagegetclip(GdImage $image): array {}

#ifdef HAVE_GD_FREETYPE
/**
 * @return array<int, int>|false
 * @refcount 1
 */
function imageftbbox(float $size, float $angle, string $font_filename, string $string, array $options = []): array|false {}

/**
 * @return array<int, int>|false
 * @refcount 1
 */
function imagefttext(GdImage $image, float $size, float $angle, int $x, int $y, int $color, string $font_filename, string $text, array $options = []): array|false {}

/**
 * @return array<int, int>|false
 * @alias imageftbbox
 */
function imagettfbbox(float $size, float $angle, string $font_filename, string $string, array $options = []): array|false {}

/**
 * @return array<int, int>|false
 * @alias imagefttext
 */
function imagettftext(GdImage $image, float $size, float $angle, int $x, int $y, int $color, string $font_filename, string $text, array $options = []): array|false {}
#endif

/** @param array|int|float|bool $args */
function imagefilter(GdImage $image, int $filter, ...$args): bool {}

function imageconvolution(GdImage $image, array $matrix, float $divisor, float $offset): bool {}

function imageflip(GdImage $image, int $mode): bool {}

function imageantialias(GdImage $image, bool $enable): bool {}

/** @refcount 1 */
function imagecrop(GdImage $image, array $rectangle): GdImage|false {}

/** @refcount 1 */
function imagecropauto(GdImage $image, int $mode = IMG_CROP_DEFAULT, float $threshold = 0.5, int $color = -1): GdImage|false {}

/** @refcount 1 */
function imagescale(GdImage $image, int $width, int $height = -1, int $mode = IMG_BILINEAR_FIXED): GdImage|false {}

/** @refcount 1 */
function imageaffine(GdImage $image, array $affine, ?array $clip = null): GdImage|false {}

/**
 * @param array|float $options
 * @refcount 1
 * @return array<int, float>|false
 */
function imageaffinematrixget(int $type, $options): array|false {}

/**
 * @return array<int, float>|false
 * @refcount 1
 */
function imageaffinematrixconcat(array $matrix1, array $matrix2): array|false {}

function imagegetinterpolation(GdImage $image): int {}

function imagesetinterpolation(GdImage $image, int $method = IMG_BILINEAR_FIXED): bool {}

/**
 * @return array<int, int>|true
 * @refcount 1
 */
function imageresolution(GdImage $image, ?int $resolution_x = null, ?int $resolution_y = null): array|bool {}
