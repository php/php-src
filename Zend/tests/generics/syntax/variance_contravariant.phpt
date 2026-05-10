--TEST--
Generic syntax: contravariant type parameter (-T)
--FILE--
<?php
class Consumer<-T> {}
$p = (new ReflectionClass('Consumer'))->getGenericParameters()[0];
echo $p->getName(), "\n";
echo $p->getVariance()->name, "\n";
?>
--EXPECT--
T
Contravariant
