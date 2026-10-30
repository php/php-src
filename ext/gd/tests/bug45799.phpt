--TEST--
Bug #45799 (imagepng() crashes on empty image).
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
$img = imagecreate(500,500);
imagepng($img);
imagedestroy($img);
?>
--EXPECTF--
Warning: imagepng(): gd-png error: no colors in palette%win %s on line %d
