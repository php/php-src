<?php

/** @generate-function-entries */

final class GdImage
{
}

function gd_info(): array {}

function imageloadfont(string $filename): int|false {}

function imagesetstyle(GdImage $image, array $style): bool {}

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

function imagegrabwindow(int $handle, bool $client_area = false): GdImage|false {}

function imagegrabscreen(): GdImage|false {}

#endif

// TODO: $ignore_transparent is ignored???
function imagerotate(GdImage $image, float $angle, int $background_color, bool $ignore_transparent = false): GdImage|false {}

function imagesettile(GdImage $image, GdImage $tile): bool {}

function imagesetbrush(GdImage $image, GdImage $brush): bool {}

function imagecreate(int $width, int $height): GdImage|false {}

function imagetypes(): int {}

function imagecreatefromstring(string $data): GdImage|false {}

function imagecreatefromgif(string $filename): GdImage|false {}

#ifdef HAVE_GD_JPG
function imagecreatefromjpeg(string $filename): GdImage|false {}
#endif

#ifdef HAVE_GD_PNG
function imagecreatefrompng(string $filename): GdImage|false {}
#endif

#ifdef HAVE_GD_WEBP
function imagecreatefromwebp(string $filename): GdImage|false {}
#endif

function imagecreatefromxbm(string $filename): GdImage|false {}

#ifdef HAVE_GD_XPM
function imagecreatefromxpm(string $filename): GdImage|false {}
#endif

function imagecreatefromwbmp(string $filename): GdImage|false {}

function imagecreatefromgd(string $filename): GdImage|false {}

function imagecreatefromgd2(string $filename): GdImage|false {}

function imagecreatefromgd2part(string $filename, int $x, int $y, int $width, int $height): GdImage|false {}

#ifdef HAVE_GD_BMP
function imagecreatefrombmp(string $filename): GdImage|false {}
#endif

#ifdef HAVE_GD_TGA
function imagecreatefromtga(string $filename): GdImage|false {}
#endif

function imagexbm(GdImage $image, ?string $filename, ?int $foreground_color = null): bool {}

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

function imagecolorset(GdImage $image, int $color, int $red, int $green, int $blue, int $alpha = 0): ?bool {}

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

function imagefontwidth(int $font): int {}

function imagefontheight(int $font): int {}

function imagechar(GdImage $image, int $font, int $x, int $y, string $char, int $color): bool {}

function imagecharup(GdImage $image, int $font, int $x, int $y, string $char, int $color): bool {}

function imagestring(GdImage $image, int $font, int $x, int $y, string $string, int $color): bool {}

function imagestringup(GdImage $image, int $font, int $x, int $y, string $string, int $color): bool {}

function imagecopy(GdImage $dst_image, GdImage $src_image, int $dst_x, int $dst_y, int $src_x, int $src_y, int $src_width, int $src_height): bool {}

function imagecopymerge(GdImage $dst_image, GdImage $src_image, int $dst_x, int $dst_y, int $src_x, int $src_y, int $src_width, int $src_height, int $pct): bool {}

function imagecopymergegray(GdImage $dst_image, GdImage $src_image, int $dst_x, int $dst_y, int $src_x, int $src_y, int $src_width, int $src_height, int $pct): bool {}

function imagecopyresized(GdImage $dst_image, GdImage $src_image, int $dst_x, int $dst_y, int $src_x, int $src_y, int $dst_width, int $dst_height, int $src_width, int $src_height): bool {}

function imagesx(GdImage $image): int {}

function imagesy(GdImage $image): int {}

function imagesetclip(GdImage $image, int $x1, int $y1, int $x2, int $y2): bool {}

function imagegetclip(GdImage $image): array {}

#ifdef HAVE_GD_FREETYPE
function imageftbbox(float $size, float $angle, string $font_filename, string $string, array $options = []): array|false {}

function imagefttext(GdImage $image, float $size, float $angle, int $x, int $y, int $color, string $font_filename, string $text, array $options = []): array|false {}

/** @alias imageftbbox */
function imagettfbbox(float $size, float $angle, string $font_filename, string $string, array $options = []): array|false {}

/** @alias imagefttext */
function imagettftext(GdImage $image, float $size, float $angle, int $x, int $y, int $color, string $font_filename, string $text, array $options = []): array|false {}
#endif

/** @param array|int|float|bool $args */
function imagefilter(GdImage $image, int $filter, ...$args): bool {}

function imageconvolution(GdImage $image, array $matrix, float $divisor, float $offset): bool {}

function imageflip(GdImage $image, int $mode): bool {}

function imageantialias(GdImage $image, bool $enable): bool {}

function imagecrop(GdImage $image, array $rectangle): GdImage|false {}

function imagecropauto(GdImage $image, int $mode = IMG_CROP_DEFAULT, float $threshold = 0.5, int $color = -1): GdImage|false {}

function imagescale(GdImage $image, int $width, int $height = -1, int $mode = IMG_BILINEAR_FIXED): GdImage|false {}

function imageaffine(GdImage $image, array $affine, ?array $clip = null): GdImage|false {}

/** @param array|float $options */
function imageaffinematrixget(int $type, $options): array|false {}

function imageaffinematrixconcat(array $matrix1, array $matrix2): array|false {}

function imagegetinterpolation(GdImage $image): int {}

function imagesetinterpolation(GdImage $image, int $method = IMG_BILINEAR_FIXED): bool {}

function imageresolution(GdImage $image, ?int $resolution_x = null, ?int $resolution_y = null): array|bool {}

