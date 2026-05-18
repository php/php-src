--TEST--
Generic syntax: closure with type parameters
--FILE--
<?php
$f = function <T>(T $x): T { return $x; };
$r = new ReflectionFunction($f);
var_dump($r->isGeneric());
echo $r->getGenericParameters()[0]->getName(), "\n";
?>
--EXPECT--
bool(true)
T
