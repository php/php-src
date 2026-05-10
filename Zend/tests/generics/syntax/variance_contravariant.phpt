--TEST--
Generic syntax: contravariant type parameter (in T)
--FILE--
<?php
class Consumer<in T> {}
$p = (new ReflectionClass('Consumer'))->getGenericParameters()[0];
echo $p->getName(), "\n";
echo $p->getVariance()->name, "\n";
?>
--EXPECT--
T
Contravariant
