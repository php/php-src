--TEST--
Bug #72604 (imagearc() ignores thickness for full arcs)
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
$im = imagecreatetruecolor(100, 100);
imagesetthickness($im, 5);
imagearc($im, 50, 50, 90, 90, 0, 360, 0xffffff);

include_once __DIR__ . '/func.inc';
test_image_equals_file(__DIR__ . '/bug72604.png', $im);
?>
--EXPECT--
The images are equal.
