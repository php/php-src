--TEST--
Generic syntax: arrow function with type parameters
--FILE--
<?php
$f = fn<T>(T $x): T => $x;
$r = new ReflectionFunction($f);
var_dump($r->isGeneric());
echo $r->getGenericParameters()[0]->getName(), "\n";
?>
--EXPECT--
bool(true)
T
