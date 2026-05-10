--TEST--
Reflection: ReflectionGenericTypeParameter::hasBound()
--FILE--
<?php
class A<X, Y : object> {}
$ps = (new ReflectionClass('A'))->getGenericParameters();
echo $ps[0]->getName(), ": ", var_export($ps[0]->hasBound(), true), "\n";
echo $ps[1]->getName(), ": ", var_export($ps[1]->hasBound(), true), "\n";
?>
--EXPECT--
X: false
Y: true
