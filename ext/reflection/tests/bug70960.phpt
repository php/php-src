--TEST--
Bug #70960 (ReflectionFunction for array_unique returns wrong number of parameters)
--FILE--
<?php

$ref = new ReflectionFunction('array_unique');
var_dump(count($ref->getParameters()));
?>
--EXPECT--
int(2)
