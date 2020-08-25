--TEST--
libgd #94 (imagecreatefromxbm can crash if gdImageCreate fails)
--SKIPIF--
<?php
	if (!extension_loaded('gd')) die("skip gd extension not available\n");
	if (!GD_BUNDLED) die("skip requires bundled GD library\n");
?>
--FILE--
<?php
$im = imagecreatefromxbm(__DIR__ . '/libgd00094.xbm');
var_dump($im);
?>
--EXPECTF--
Warning: imagecreatefromxbm(): '%slibgd00094.xbm' is not a valid XBM file in %slibgd00094.php on line %d
bool(false)
