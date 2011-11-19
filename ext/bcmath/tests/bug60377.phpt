--TEST--
bcscale related problem on 64bits platforms
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--FILE--
<?php
$var48 = bcscale(634314234334311);
$var67 = bcsqrt(false);
$var414 = bcadd(false,null,10);
die('ALIVE');
?>
--EXPECTF--
ALIVE
