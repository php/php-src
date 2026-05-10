--TEST--
Erasure: composite bound erased
--FILE--
<?php
class A {}
function f<T : A|int>(T $x): T { return $x; }
$rt = (new ReflectionFunction('f'))->getReturnType();
echo get_class($rt), "\n";
?>
--EXPECT--
ReflectionUnionType
