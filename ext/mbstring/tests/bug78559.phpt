--TEST--
Bug #78559 (#78559	Heap buffer overflow in mb_eregi)
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
if (!function_exists('mb_ereg')) die('skip mb_ereg() not available');
?>
--FILE--
<?php
$str = "5b5b5b5b5b5b5b492a5bce946b5c4b5d5c6b5c4b5d5c4b5d1cceb04b5d1cceb07a73717e4b1c52525252525252525252525252525252525252525252525252492a5bce946b5c4b5d5c6b5c4b5d5c4b5d1cceb04b5d1cceb07a73717e4b1c1cceb04b5d1cceb07a73717e4b1c302c36303030ceb07b7bd2a15c305c30663f436f6e74655c5238416711087b363030302c36303030ceb07b7b7b7b7b7b7b363030302c36303030ceb07b7b7b7b7b7b7b4a01";
$str = hex2bin($str);
var_dump(mb_eregi($str, $str));
?>
--EXPECT--
bool(false)
