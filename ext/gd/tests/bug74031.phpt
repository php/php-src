--TEST--
(Bug #74031) ReflectionFunction for imagepng returns wrong number of parameters
--SKIPIF--
<?php
if (!extension_loaded('gd')) die("skip gd extension not available\n");
?>
--FILE--
<?php

$ref = new ReflectionFunction('imagepng');
var_dump(count($ref->getParameters()));
?>
--EXPECT--
int(4)
