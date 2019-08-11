<?php

function gd_info(): array {}

/** @return int|false */
function imageloadfont(string $filename) {}

function imagesetstyle($im, array $styles): bool {}

/** @return resource|false */
function imagecreatetruecolor(int $x_size, int $y_size) {}

function imageistruecolor($im): bool {}

function imagetruecolortopalette($im, bool $ditherFlag, int $colorWanted): bool {}

function imagepalettetotruecolor($im): bool {}

function imagecolormatch($im1, $im2): bool {}

function imagesetthickness($im, int $thickness): bool {}

function imagefilledellipse($im, int $cx, int $cy, int $w, int $h, int $color): bool {}

function imagefilledarc($im, int $cx, int $cy, int $w, int $h, int $s, int $e, int $col, int $style): bool {}

function imagealphablending($im, bool $blend): bool {}

function imagesavealpha($im, bool $save): bool {}

function imagelayereffect($im, int $effect): bool {}

/** @return int|false */
function imagecolorallocatealpha($im, int $red, int $green, int $blue, int $alpha) {}

/** @return int|false */
function imagecolorresolvealpha($im, int $red, int $green, int $blue, int $alpha) {}

/** @return int|false */
function imagecolorclosestalpha($im, int $red, int $green, int $blue, int $alpha) {}

/** @return int|false */
function imagecolorexactalpha($im, int $red, int $green, int $blue, int $alpha) {}

function imagecopyresampled($dst_im, $src_im, int $dst_x, int $dst_y, int $src_x, int $src_y, int $dst_w, int $dst_h, int $src_w, int $src_h): bool {}

#ifdef PHP_WIN32

/** @return resource|false */
function imagegrabwindow(int $handle, int $client_area = 0) {}

/** @return resource|false */
function imagegrabscreen() {}

#endif

/** @return resource|false */
function imagerotate($im, float $angle, int $bgdcolor, int $ignoretransparent = 0) {}

function imagesettile($im, $tile): bool {}

function imagesetbrush($im, $brush): bool {}

/** @return resource|false */
function imagecreate(int $x_size, int $y_size) {}

function imagetypes(): int {}

/** @return resource|false */
function imagecreatefromstring(string $image) {}

/** @return resource|false */
function imagecreatefromgif(string $filename) {}

#ifdef HAVE_GD_JPG
/** @return resource|false */
function imagecreatefromjpeg(string $filename) {}
#endif

#ifdef HAVE_GD_PNG
/** @return resource|false */
function imagecreatefrompng(string $filename) {}
#endif

#ifdef HAVE_GD_WEBP
/** @return resource|false */
function imagecreatefromwebp(string $filename) {}
#endif

/** @return resource|false */
function imagecreatefromxbm(string $filename) {}

#ifdef HAVE_GD_XPM
/** @return resource|false */
function imagecreatefromxpm(string $filename) {}
#endif

/** @return resource|false */
function imagecreatefromwbmp(string $filename) {}

/** @return resource|false */
function imagecreatefromgd(string $filename) {}

/** @return resource|false */
function imagecreatefromgd2(string $filename) {}

/** @return resource|false */
function imagecreatefromgd2part(string $filename, int $srcX, int $srcY, int $width, int $height) {}

#ifdef HAVE_GD_BMP
/** @return resource|false */
function imagecreatefrombmp(string $filename) {}
#endif
    
#ifdef HAVE_GD_TGA
/** @return resource|false */
function imagecreatefromtga(string $filename) {}
#endif

function imagexbm($im, string $filename, int $foreground = UNKNOWN): bool {}

function imagegif($im, $to = NULL): bool {}

#ifdef HAVE_GD_PNG
function imagepng($im, $to = NULL, int $quality = UNKNOWN, int $filters = UNKNOWN): bool {}
#endif

#ifdef HAVE_GD_WEBP
function imagewebp($im, $to = NULL, int $quality = UNKNOWN): bool {}
#endif

#ifdef HAVE_GD_JPG
function imagejpeg($im, $to = NULL, int $quality = UNKNOWN): bool {}
#endif

function imagewbmp($im, $to = NULL, int $foreground = UNKNOWN): bool {}

function imagegd($im, $to = UNKNOWN): bool {}

function imagegd2($im, $to = UNKNOWN, int $chunk_size = UNKNOWN, int $type = UNKNOWN): bool {}

#ifdef HAVE_GD_BMP
function imagebmp($im, $to = NULL, int $compressed = 1): bool {}
#endif

function imagedestroy($im): bool {}

/** @return int|false */
function imagecolorallocate($im, int $red, int $green, int $blue) {}

function imagepalettecopy($dst, $src): void {}

/** @return int|false */
function imagecolorat($im, int $x, int $y) {}

/** @return int|false */
function imagecolorclosest($im, int $red, int $green, int $blue) {}

/** @return int|false */
function imagecolorclosesthwb($im, int $red, int $green, int $blue) {}

function imagecolordeallocate($im, int $index): bool {}

/** @return int|false */
function imagecolorresolve($im, int $red, int $green, int $blue) {}

/** @return int|false */
function imagecolorexact($im, int $red, int $green, int $blue) {}

/** @return ?false */
function imagecolorset($im, int $color, int $red, int $green, int $blue, int $alpha = 0) {}

/** @return array|false */
function imagecolorsforindex($im, int $index) {}

function imagegammacorrect($im, float $inputgamma, float $outputgamma): bool {}

function imagesetpixel($im, int $x, int $y, int $col): bool {}

function imageline($im, int $x1, int $y1, int $x2, int $y2, int $col): bool {}

function imagedashedline($im, int $x1, int $y1, int $x2, int $y2, int $col): bool {}

function imagerectangle($im, int $x1, int $y1, int $x2, int $y2, int $col): bool {}

function imagefilledrectangle($im, int $x1, int $y1, int $x2, int $y2, int $col): bool {}

function imagearc($im, int $cx, int $cy, int $w, int $h, int $s, int $e, int $col): bool {}

function imageellipse($im, int $cx, int $cy, int $w, int $h, int $color): bool {}

function imagefilltoborder($im, int $x, int $y, int $border, int $col): bool {}

function imagefill($im, int $x, int $y, int $col): bool {}

function imagecolorstotal($im): int {}

function imagecolortransparent($im, int $col = UNKNOWN): ?int {}

function imageinterlace($im, int $interlace = UNKNOWN): ?int {}

function imagepolygon($im, array $points, int $num_pos, int $col): bool {}

function imageopenpolygon($im, array $points, int $num_pos, int $col): bool {}

function imagefilledpolygon($im, array $points, int $num_pos, int $col): bool {}

function imagefontwidth(int $font): int {}

function imagefontheight(int $font): int {}

function imagechar($im, int $font, int $x, int $y, string $c, int $col): bool {}

function imagecharup($im, int $font, int $x, int $y, string $c, int $col): bool {}

function imagestring($im, int $font, int $x, int $y, string $str, int $col): bool {}

function imagestringup($im, int $font, int $x, int $y, string $str, int $col): bool {}

function imagecopy($dst_im, $src_im, int $dst_x, int $dst_y, int $src_x, int $src_y, int $src_w, int $src_h): bool {}

function imagecopymerge($dst_im, $src_im, int $dst_x, int $dst_y, int $src_x, int $src_y, int $src_w, int $src_h, int $pct): bool {}

function imagecopymergegray($dst_im, $src_im, int $dst_x, int $dst_y, int $src_x, int $src_y, int $src_w, int $src_h, int $pct): bool {}

function imagecopyresized($dst_im, $src_im, int $dst_x, int $dst_y, int $src_x, int $src_y, int $dst_w, int $dst_h, int $src_w, int $src_h): bool {}

function imagesx($im): int {}

function imagesy($im): int {}

function imagesetclip($im, int $x1, int $x2, int $y1, int $y2): bool {}

function imagegetclip($im): array {}

#ifdef HAVE_GD_FREETYPE
/** @return array|false */
function imageftbbox(float $size, float $angle, string $font_file, string $text, array $extrainfo = UNKNOWN) {}

function imagefttext($im, float $size, float $angle, int $x, int $y, int $col, string $font_file, string $text, array $extrainfo = UNKNOWN) {}

function imagettfbbox(float $size, float $angle, string $font_file, string $text) {}

function imagettftext($im, float $size, float $angle, int $x, int $y, int $col, string $font_file, string $text) {}
#endif

function imagefilter($im, int $filtertype, $arg1 = UNKNOWN, $arg2 = UNKNOWN, $arg3 = UNKNOWN, $arg4 = UNKNOWN): bool {}

function imageconvolution($im, array $matrix3x3, float $div, float $offset): bool {}

function imageflip($im, int $mode): bool {}

function imageantialias($im, bool $on): bool {}

/** @return resource|false */
function imagecrop($im, array $rect) {}

/** @return resource|false */
function imagecropauto($im, int $mode = IMG_CROP_DEFAULT, float $threshold = 0.5, int $color = -1) {}

/** @return resource|false */
function imagescale($im, int $new_width, int $new_height = UNKNOWN, int $mode = IMG_BILINEAR_FIXED) {}

/** @return resource|false */
function imageaffine($im, array $affine, array $clip = UNKNOWN) {}

/** @return array|false */
function imageaffinematrixget(int $type, $options = UNKNOWN) {}

/** @return array|false */
function imageaffinematrixconcat(array $m1, array $m2) {}

function imagesetinterpolation($im, int $method = IMG_BILENEAR_FIXED): bool {}

/** @return array|true */
function imageresolution($im, int $res_x = UNKNOWN, int $res_y = UNKNOWN) {}
