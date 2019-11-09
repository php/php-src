<?php

function gd_info(): array {}

/** @return int|false */
function imageloadfont(string $filename) {}

function imagesetstyle($im, array $styles): bool {}

/** @return GdImage|false */
function imagecreatetruecolor(int $x_size, int $y_size) {}

function imageistruecolor(GdImage $im): bool {}

function imagetruecolortopalette(GdImage $im, bool $ditherFlag, int $colorWanted): bool {}

function imagepalettetotruecolor(GdImage $im): bool {}

function imagecolormatch($im1, $im2): bool {}

function imagesetthickness(GdImage $im, int $thickness): bool {}

function imagefilledellipse(GdImage $im, int $cx, int $cy, int $w, int $h, int $color): bool {}

function imagefilledarc(GdImage $im, int $cx, int $cy, int $w, int $h, int $s, int $e, int $col, int $style): bool {}

function imagealphablending(GdImage $im, bool $blend): bool {}

function imagesavealpha(GdImage $im, bool $save): bool {}

function imagelayereffect(GdImage $im, int $effect): bool {}

/** @return int|false */
function imagecolorallocatealpha(GdImage $im, int $red, int $green, int $blue, int $alpha) {}

/** @return int|false */
function imagecolorresolvealpha(GdImage $im, int $red, int $green, int $blue, int $alpha) {}

/** @return int|false */
function imagecolorclosestalpha(GdImage $im, int $red, int $green, int $blue, int $alpha) {}

/** @return int|false */
function imagecolorexactalpha(GdImage $im, int $red, int $green, int $blue, int $alpha) {}

function imagecopyresampled(GdImage $dst_im, GdImage $src_im, int $dst_x, int $dst_y, int $src_x, int $src_y, int $dst_w, int $dst_h, int $src_w, int $src_h): bool {}

#ifdef PHP_WIN32

/** @return GdImage|false */
function imagegrabwindow(int $handle, int $client_area = 0) {}

/** @return GdImage|false */
function imagegrabscreen() {}

#endif

/** @return GdImage|false */
function imagerotate(GdImage $im, float $angle, int $bgdcolor, int $ignoretransparent = 0) {}

function imagesettile(GdImage $im, $tile): bool {}

function imagesetbrush(GdImage $im, $brush): bool {}

/** @return GdImage|false */
function imagecreate(int $x_size, int $y_size) {}

function imagetypes(): int {}

/** @return GdImage|false */
function imagecreatefromstring(string $image) {}

/** @return GdImage|false */
function imagecreatefromgif(string $filename) {}

#ifdef HAVE_GD_JPG
/** @return GdImage|false */
function imagecreatefromjpeg(string $filename) {}
#endif

#ifdef HAVE_GD_PNG
/** @return GdImage|false */
function imagecreatefrompng(string $filename) {}
#endif

#ifdef HAVE_GD_WEBP
/** @return GdImage|false */
function imagecreatefromwebp(string $filename) {}
#endif

/** @return GdImage|false */
function imagecreatefromxbm(string $filename) {}

#ifdef HAVE_GD_XPM
/** @return GdImage|false */
function imagecreatefromxpm(string $filename) {}
#endif

/** @return GdImage|false */
function imagecreatefromwbmp(string $filename) {}

/** @return GdImage|false */
function imagecreatefromgd(string $filename) {}

/** @return GdImage|false */
function imagecreatefromgd2(string $filename) {}

/** @return GdImage|false */
function imagecreatefromgd2part(string $filename, int $srcX, int $srcY, int $width, int $height) {}

#ifdef HAVE_GD_BMP
/** @return GdImage|false */
function imagecreatefrombmp(string $filename) {}
#endif
    
#ifdef HAVE_GD_TGA
/** @return GdImage|false */
function imagecreatefromtga(string $filename) {}
#endif

function imagexbm(GdImage $im, ?string $filename, int $foreground = UNKNOWN): bool {}

function imagegif(GdImage $im, $to = NULL): bool {}

#ifdef HAVE_GD_PNG
function imagepng(GdImage $im, $to = NULL, int $quality = UNKNOWN, int $filters = UNKNOWN): bool {}
#endif

#ifdef HAVE_GD_WEBP
function imagewebp(GdImage $im, $to = NULL, int $quality = UNKNOWN): bool {}
#endif

#ifdef HAVE_GD_JPG
function imagejpeg(GdImage $im, $to = NULL, int $quality = UNKNOWN): bool {}
#endif

function imagewbmp(GdImage $im, $to = NULL, int $foreground = UNKNOWN): bool {}

function imagegd(GdImage $im, $to = UNKNOWN): bool {}

function imagegd2(GdImage $im, $to = UNKNOWN, int $chunk_size = UNKNOWN, int $type = UNKNOWN): bool {}

#ifdef HAVE_GD_BMP
function imagebmp(GdImage $im, $to = NULL, int $compressed = 1): bool {}
#endif

function imagedestroy(GdImage $im): bool {}

/** @return int|false */
function imagecolorallocate(GdImage $im, int $red, int $green, int $blue) {}

function imagepalettecopy(GdImage $dst, GdImage $src): void {}

/** @return int|false */
function imagecolorat(GdImage $im, int $x, int $y) {}

/** @return int|false */
function imagecolorclosest(GdImage $im, int $red, int $green, int $blue) {}

/** @return int|false */
function imagecolorclosesthwb(GdImage $im, int $red, int $green, int $blue) {}

function imagecolordeallocate(GdImage $im, int $index): bool {}

/** @return int|false */
function imagecolorresolve(GdImage $im, int $red, int $green, int $blue) {}

/** @return int|false */
function imagecolorexact(GdImage $im, int $red, int $green, int $blue) {}

/** @return ?false */
function imagecolorset(GdImage $im, int $color, int $red, int $green, int $blue, int $alpha = 0) {}

/** @return array|false */
function imagecolorsforindex(GdImage $im, int $index) {}

function imagegammacorrect(GdImage $im, float $inputgamma, float $outputgamma): bool {}

function imagesetpixel(GdImage $im, int $x, int $y, int $col): bool {}

function imageline(GdImage $im, int $x1, int $y1, int $x2, int $y2, int $col): bool {}

function imagedashedline(GdImage $im, int $x1, int $y1, int $x2, int $y2, int $col): bool {}

function imagerectangle(GdImage $im, int $x1, int $y1, int $x2, int $y2, int $col): bool {}

function imagefilledrectangle(GdImage $im, int $x1, int $y1, int $x2, int $y2, int $col): bool {}

function imagearc(GdImage $im, int $cx, int $cy, int $w, int $h, int $s, int $e, int $col): bool {}

function imageellipse(GdImage $im, int $cx, int $cy, int $w, int $h, int $color): bool {}

function imagefilltoborder(GdImage $im, int $x, int $y, int $border, int $col): bool {}

function imagefill(GdImage $im, int $x, int $y, int $col): bool {}

function imagecolorstotal(GdImage $im): int {}

function imagecolortransparent(GdImage $im, int $col = UNKNOWN): ?int {}

function imageinterlace(GdImage $im, int $interlace = UNKNOWN): ?int {}

function imagepolygon(GdImage $im, array $points, int $num_pos, int $col): bool {}

function imageopenpolygon(GdImage $im, array $points, int $num_pos, int $col): bool {}

function imagefilledpolygon(GdImage $im, array $points, int $num_pos, int $col): bool {}

function imagefontwidth(int $font): int {}

function imagefontheight(int $font): int {}

function imagechar(GdImage $im, int $font, int $x, int $y, string $c, int $col): bool {}

function imagecharup(GdImage $im, int $font, int $x, int $y, string $c, int $col): bool {}

function imagestring(GdImage $im, int $font, int $x, int $y, string $str, int $col): bool {}

function imagestringup(GdImage $im, int $font, int $x, int $y, string $str, int $col): bool {}

function imagecopy(GdImage $dst_im, GdImage $src_im, int $dst_x, int $dst_y, int $src_x, int $src_y, int $src_w, int $src_h): bool {}

function imagecopymerge(GdImage $dst_im, GdImage $src_im, int $dst_x, int $dst_y, int $src_x, int $src_y, int $src_w, int $src_h, int $pct): bool {}

function imagecopymergegray(GdImage $dst_im, GdImage $src_im, int $dst_x, int $dst_y, int $src_x, int $src_y, int $src_w, int $src_h, int $pct): bool {}

function imagecopyresized(GdImage $dst_im, GdImage $src_im, int $dst_x, int $dst_y, int $src_x, int $src_y, int $dst_w, int $dst_h, int $src_w, int $src_h): bool {}

function imagesx(GdImage $im): int {}

function imagesy(GdImage $im): int {}

function imagesetclip(GdImage $im, int $x1, int $x2, int $y1, int $y2): bool {}

function imagegetclip(GdImage $im): array {}

#ifdef HAVE_GD_FREETYPE
/** @return array|false */
function imageftbbox(float $size, float $angle, string $font_file, string $text, array $extrainfo = UNKNOWN) {}

function imagefttext(GdImage $im, float $size, float $angle, int $x, int $y, int $col, string $font_file, string $text, array $extrainfo = UNKNOWN) {}

function imagettfbbox(float $size, float $angle, string $font_file, string $text) {}

function imagettftext(GdImage $im, float $size, float $angle, int $x, int $y, int $col, string $font_file, string $text) {}
#endif

function imagefilter(GdImage $im, int $filtertype, $arg1 = UNKNOWN, $arg2 = UNKNOWN, $arg3 = UNKNOWN, $arg4 = UNKNOWN): bool {}

function imageconvolution(GdImage $im, array $matrix3x3, float $div, float $offset): bool {}

function imageflip(GdImage $im, int $mode): bool {}

function imageantialias(GdImage $im, bool $on): bool {}

/** @return GdImage|false */
function imagecrop(GdImage $im, array $rect) {}

/** @return GdImage|false */
function imagecropauto(GdImage $im, int $mode = IMG_CROP_DEFAULT, float $threshold = 0.5, int $color = -1) {}

/** @return GdImage|false */
function imagescale(GdImage $im, int $new_width, int $new_height = UNKNOWN, int $mode = IMG_BILINEAR_FIXED) {}

/** @return GdImage|false */
function imageaffine(GdImage $im, array $affine, array $clip = UNKNOWN) {}

/** @return array|false */
function imageaffinematrixget(int $type, $options = UNKNOWN) {}

/** @return array|false */
function imageaffinematrixconcat(array $m1, array $m2) {}

function imagesetinterpolation(GdImage $im, int $method = IMG_BILENEAR_FIXED): bool {}

/** @return array|true */
function imageresolution(GdImage $im, int $res_x = UNKNOWN, int $res_y = UNKNOWN) {}
