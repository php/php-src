--TEST--
Generic syntax: covariant type parameter (out T)
--FILE--
<?php
class Producer<out T> {}
$p = (new ReflectionClass('Producer'))->getGenericParameters()[0];
echo $p->getName(), "\n";
echo $p->getVariance()->name, "\n";
?>
--EXPECT--
T
Covariant
