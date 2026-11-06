--TEST--
imagewebp() with a palette image
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!function_exists('imagewebp')) {
    die('skip WebP support is not available');
}
?>
--FILE--
<?php
$im = imagecreate(4, 4);
var_dump(imagewebp($im, __DIR__ . '/imagewebp_palette.webp'));

$im = imagecreatetruecolor(16384, 1);
var_dump(imagewebp($im, __DIR__ . '/imagewebp_truecolor.webp'));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/imagewebp_palette.webp');
@unlink(__DIR__ . '/imagewebp_truecolor.webp');
?>
--EXPECTF--
Warning: imagewebp(): Palette image not supported by webp in %s on line %d
bool(false)

Warning: imagewebp(): gd-webp encoding failed in %s on line %d
bool(false)