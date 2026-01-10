--TEST--
Bug #73213 (Integer overflow in imageline() with antialiasing)
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!(imagetypes() & IMG_PNG)) {
    die("skip No PNG support");
}
?>
--FILE--
<?php
require_once __DIR__ . DIRECTORY_SEPARATOR . 'func.inc';

$im = imagecreatetruecolor(32768, 1);
$black = imagecolorallocate($im, 0, 0, 0);
imageantialias($im, true);

imageline($im, 0,0, 32767,0, $black);

test_image_equals_file(__DIR__ . DIRECTORY_SEPARATOR . 'bug73213.png', $im);
?>
--EXPECT--
The images are equal.
