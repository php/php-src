--TEST--
Bug #77973 (Uninitialized read in gdImageCreateFromXbm)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die("skip gd extension not available");
if (!function_exists('imagecreatefromxbm')) die("skip imagecreatefromxbm not available");
?>
--FILE--
<?php
$contents = hex2bin("23646566696e6520776964746820320a23646566696e652068656967687420320a737461746963206368617220626974735b5d203d7b0a7a7a787a7a");
$filepath = __DIR__ . '/bug77973.xbm';
file_put_contents($filepath, $contents);
$im = imagecreatefromxbm($filepath);
var_dump($im);
?>
--EXPECTF--
Warning: imagecreatefromxbm(): Invalid XBM in %s on line %d

Warning: imagecreatefromxbm(): "%s" is not a valid XBM file in %s on line %d
bool(false)
--CLEAN--
<?php
unlink(__DIR__ . '/bug77973.xbm');
?>
