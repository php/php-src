--TEST--
Bug #37346 (gdimagecreatefromgif, bad colormap)
--SKIPIF--
<?php
    if (!extension_loaded('gd')) die("skip gd extension not available\n");
?>
--FILE--
<?php
$im = imagecreatefromgif(__DIR__ . '/bug37346私はガラスを食べられます.gif');
?>
--EXPECTF--
Warning: imagecreatefromgif(): "%sbug37346私はガラスを食べられます.gif" is not a valid GIF file in %sbug37346-mb.php on line %d
