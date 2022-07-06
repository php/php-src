--TEST--
imagetypes() - TGA support
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip ext/gd required');
?>
--FILE--
<?php
var_dump((imagetypes() & IMG_TGA) == function_exists('imagecreatefromtga'));
?>
--EXPECT--
bool(true)
