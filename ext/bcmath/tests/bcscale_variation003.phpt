--TEST--
bcscale() return value
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--INI--
bcmath.scale=0
--FILE--
<?php
var_dump(bcscale(1));
var_dump(bcscale(4));
var_dump(bcscale());
var_dump(bcscale());
?>
--EXPECT--
int(0)
int(1)
int(4)
int(4)
