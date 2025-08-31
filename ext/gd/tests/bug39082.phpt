--TEST--
Bug #39082 (Output image to stdout segfaults).
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreatetruecolor(1,1);
imagegif($im);
?>
--EXPECTF--
GIF87a%s
