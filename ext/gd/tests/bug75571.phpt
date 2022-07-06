--TEST--
Bug #75571 (Infinite loop in GIF reading causing DoS)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
var_dump(imagecreatefromgif(__DIR__ . '/bug75571.gif'));
?>
--EXPECTF--
Warning: imagecreatefromgif(): "%s" is not a valid GIF file in %s on line %d
bool(false)
