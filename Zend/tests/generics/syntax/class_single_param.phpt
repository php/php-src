--TEST--
Generic syntax: class with single type parameter
--FILE--
<?php
class Box<T> {}
$r = new ReflectionClass('Box');
var_dump($r->isGeneric());
echo count($r->getGenericParameters()), "\n";
echo $r->getGenericParameters()[0]->getName(), "\n";
?>
--EXPECT--
bool(true)
1
T
