--TEST--
bcscale related problem on 64bits platforms
--SKIPIF--
<?php if(!extension_loaded("bcmath")) die("skip");
if (PHP_INT_SIZE != 8) die("skip: 64-bit only"); ?>
--FILE--
<?php
$var48 = bcscale(634314234334311);
$var67 = bcsqrt(0);
$var414 = bcadd(0,-1,10);
die('ALIVE');
?>
--EXPECT--
ALIVE
