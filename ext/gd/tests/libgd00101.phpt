--TEST--
libgd #101 (imagecreatefromgd can crash if gdImageCreate fails)
--SKIPIF--
<?php
    if (!extension_loaded('gd')) die("skip gd extension not available\n");
    if (!GD_BUNDLED) die("skip requires bundled GD library\n");
?>
--FILE--
<?php
$im = imagecreatefromgd(__DIR__ . '/libgd00101.gd');
var_dump($im);
?>
--EXPECTF--
Warning: imagecreatefromgd(): Product of memory allocation multiplication would exceed INT_MAX, failing operation gracefully
 in %slibgd00101.php on line %d

Warning: imagecreatefromgd(): "%slibgd00101.gd" is not a valid GD file in %slibgd00101.php on line %d
bool(false)
