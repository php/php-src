--TEST--
Reflection: ReflectionGenericTypeParameter::getPosition()
--FILE--
<?php
class A<X, Y, Z> {}
$ps = (new ReflectionClass('A'))->getGenericParameters();
foreach ($ps as $p) {
    echo $p->getName(), ": ", $p->getPosition(), "\n";
}
?>
--EXPECT--
X: 0
Y: 1
Z: 2
