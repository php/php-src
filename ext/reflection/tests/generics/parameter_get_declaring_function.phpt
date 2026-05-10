--TEST--
Reflection: getDeclaringEntity() returns the declaring function
--FILE--
<?php
function f<T>(): void {}
$p = (new ReflectionFunction('f'))->getGenericParameters()[0];
$de = $p->getDeclaringEntity();
echo get_class($de), "\n";
echo $de->getName(), "\n";
?>
--EXPECT--
ReflectionFunction
f
