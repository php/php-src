--TEST--
Testing imagecopymergealpha() of GD library
--CREDITS--
Matt Clegg <cleggmatt [at] gmail [dot] com>
Orignally by: redmonkey
--SKIPIF--
<?php 
if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php

$bg  = imagecreatefrompng('test.png');
imagealphablending($bg, true);
imagesavealpha($bg, true);

$over = imagecreatefrompng('conv_test.png');
var_dump(imagecopymergealpha($bg, $over, 10, 10, 0, 0, 70, 70, 50));

?>
--EXPECTF--
bool(true)
