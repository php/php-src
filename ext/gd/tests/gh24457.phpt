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
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/imagewebp_palette.webp');
?>
--EXPECTF--
Warning: imagewebp(): Palette image not supported by webp in %s on line %d
bool(false)
