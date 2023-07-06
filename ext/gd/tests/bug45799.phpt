--TEST--
Bug #45799 (imagepng() crashes on empty image).
--EXTENSIONS--
gd
--FILE--
<?php
$img = imagecreate(500,500);
imagepng($img);
imagedestroy($img);
?>
--EXPECTF--
Warning: imagepng(): gd-png error: no colors in palette%win %s on line %d
