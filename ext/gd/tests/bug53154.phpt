--TEST--
Bug #53154 (Zero-height rectangle has whiskers)
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED && version_compare(GD_VERSION, '2.2.2', '<')) {
    die("skip test requires GD 2.2.2 or higher");
}
?>
--FILE--
<?php
$im = imagecreatetruecolor(100, 10);
$red = imagecolorallocate($im, 255, 0, 0);
imagerectangle($im, 5, 5, 95, 5, $red);
var_dump(imagecolorat($im, 5, 4) !== $red);
var_dump(imagecolorat($im, 5, 6) !== $red);
var_dump(imagecolorat($im, 95, 4) !== $red);
var_dump(imagecolorat($im, 95, 6) !== $red);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
