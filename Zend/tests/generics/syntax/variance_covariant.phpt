--TEST--
Generic syntax: covariant type parameter (+T)
--FILE--
<?php
class Producer<+T> {}
$p = (new ReflectionClass('Producer'))->getGenericParameters()[0];
echo $p->getName(), "\n";
echo $p->getVariance()->name, "\n";
?>
--EXPECT--
T
Covariant
