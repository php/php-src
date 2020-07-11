--TEST--
imagetypes() - BMP support
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip ext/gd required');
?>
--FILE--
<?php
var_dump((imagetypes() & IMG_BMP) == function_exists('imagebmp'));
?>
--EXPECT--
bool(true)
