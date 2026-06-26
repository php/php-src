--TEST--
Generic syntax: interface with type parameters
--FILE--
<?php
interface Iter<T> {}
$r = new ReflectionClass('Iter');
var_dump($r->isGeneric());
echo $r->getGenericParameters()[0]->getName(), "\n";
?>
--EXPECT--
bool(true)
T
