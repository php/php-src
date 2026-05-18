--TEST--
Reflection: getGenericParameters on non-generic returns empty array
--FILE--
<?php
class A {}
function f(): void {}
var_dump((new ReflectionClass('A'))->getGenericParameters());
var_dump((new ReflectionFunction('f'))->getGenericParameters());
?>
--EXPECT--
array(0) {
}
array(0) {
}
