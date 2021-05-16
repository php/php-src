--TEST--
Bug #37346 (gdimagecreatefromgif, bad colormap)
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreatefromgif(__DIR__ . '/bug37346.gif');
?>
--EXPECTF--
Warning: imagecreatefromgif(): "%sbug37346.gif" is not a valid GIF file in %sbug37346.php on line %d
