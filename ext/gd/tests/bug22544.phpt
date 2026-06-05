--TEST--
Bug #22544 (TrueColor transparency in PNG images).
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
    $image = imagecreatetruecolor(640, 100);
    $transparent = imagecolorallocate($image, 0, 0, 0);
    $red = imagecolorallocate($image, 255, 50, 50);
    imagecolortransparent($image, $transparent);
    imagefilledrectangle($image, 0, 0, 640-1, 100-1, $transparent);
    include_once __DIR__ . '/func.inc';
    test_image_equals_file(__DIR__ . '/bug22544.png', $image);
?>
--EXPECT--
The images are equal.
