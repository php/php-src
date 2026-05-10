--TEST--
Reflection: ReflectionGenericTypeParameter::getVariance()
--FILE--
<?php
class A<+X, -Y, Z> {}
foreach ((new ReflectionClass('A'))->getGenericParameters() as $p) {
    echo $p->getName(), ": ", $p->getVariance()->name, "\n";
}
?>
--EXPECT--
X: Covariant
Y: Contravariant
Z: Invariant
