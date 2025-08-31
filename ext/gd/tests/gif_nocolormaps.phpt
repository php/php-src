--TEST--
A GIF without any Global or Local color tables is still decoded
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreatefromgif(__DIR__ . "/gif_nocolormaps.gif");
var_dump($im instanceof GdImage);
?>
--EXPECT--
bool(true)
