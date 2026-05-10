--TEST--
Generic syntax: type parameter without variance is invariant
--FILE--
<?php
class Box<T> {}
$p = (new ReflectionClass('Box'))->getGenericParameters()[0];
var_dump($p->getVariance() === ReflectionGenericVariance::Invariant);
echo $p->getVariance()->name, "\n";
?>
--EXPECT--
bool(true)
Invariant
