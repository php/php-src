--TEST--
Bug #37346 (gdimagecreatefromgif, bad colormap)
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreatefromgif(__DIR__ . '/bug37346私はガラスを食べられます.gif');
?>
--EXPECTF--
Warning: imagecreatefromgif(): "%sbug37346私はガラスを食べられます.gif" is not a valid GIF file in %sbug37346-mb.php on line %d
