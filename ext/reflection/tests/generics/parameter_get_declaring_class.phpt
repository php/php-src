--TEST--
Reflection: getDeclaringEntity() returns the declaring class
--FILE--
<?php
class A<X> {}
$p = (new ReflectionClass('A'))->getGenericParameters()[0];
$de = $p->getDeclaringEntity();
echo get_class($de), "\n";
echo $de->getName(), "\n";
?>
--EXPECT--
ReflectionClass
A
