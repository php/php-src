--TEST--
Bug #72661 (ReflectionType::__toString crashes with iterable)
--FILE--
<?php
function test(iterable $arg) { }

var_dump((string)(new ReflectionParameter("test", 0))->getType());
?>
--EXPECT--
string(8) "iterable"
