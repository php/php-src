--TEST--
Generic syntax: trait with type parameters
--FILE--
<?php
trait Container<T> {}
$r = new ReflectionClass('Container');
var_dump($r->isGeneric());
echo $r->getGenericParameters()[0]->getName(), "\n";
?>
--EXPECT--
bool(true)
T
