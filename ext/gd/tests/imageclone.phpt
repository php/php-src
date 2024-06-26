--TEST--
imageclone()
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!function_exists("imageclone")) {
   die("skip imageclone unavailable");
}
--FILE--
<?php

$orig = imagecreate(100, 100);

$color_orig = imagecolorallocate($orig, 255, 0, 1);

imagefill($orig, 0, 0, $color_orig);

$copy = imageclone($orig);

var_dump($copy === $orig);
var_dump(imagesx($copy) == imagesx($orig));
var_dump(imagesy($copy) == imagesy($orig));

$rgb_orig = imagecolorat($orig, 0, 0);
$rgb_copy = imagecolorat($copy, 0, 0);

var_dump($rgb_orig == $rgb_copy);

?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(true)
