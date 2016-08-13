--TEST--
Bug #70047 (gd_info() doesn't report WebP support)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
$info = gd_info();
var_dump(array_key_exists('WebP Support', $info));
var_dump($info['WebP Support'] === function_exists('imagewebp'));
?>
--EXPECT--
bool(true)
bool(true)
