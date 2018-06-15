--TEST--
Bug #75437 Wrong reflection on imagewebp
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
if (!GD_BUNDLED && version_compare(GD_VERSION, '2.2.0', '<')) die("skip test requires GD 2.2.0 or higher");
if (!function_exists('imagewebp')) die('skip WebP support not available');
?>
--FILE--
<?php
$rf = new ReflectionFunction('imagewebp');
var_dump($rf->getNumberOfParameters());
var_dump($rf->getNumberOfRequiredParameters());
?>
===DONE===
--EXPECT--
int(3)
int(1)
===DONE===
