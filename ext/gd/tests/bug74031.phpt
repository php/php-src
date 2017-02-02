--TEST--
(Bug #74031) ReflectionFunction for imagepng returns wrong number of parameters
--FILE--
<?php

$ref = new ReflectionFunction('imagepng');
var_dump(count($ref->getParameters()));
?>
--EXPECT--
int(4)
