--TEST--
Reflection: getGenericParameters returns correct count
--FILE--
<?php
class A<X, Y, Z> {}
$ps = (new ReflectionClass('A'))->getGenericParameters();
echo count($ps), "\n";
echo $ps[0]->getName(), "\n";
echo $ps[1]->getName(), "\n";
echo $ps[2]->getName(), "\n";
?>
--EXPECT--
3
X
Y
Z
