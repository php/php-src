--TEST--
Generic syntax: function declaration with type parameters
--FILE--
<?php
function id<T>(T $x): T { return $x; }
$r = new ReflectionFunction('id');
var_dump($r->isGeneric());
echo $r->getGenericParameters()[0]->getName(), "\n";
?>
--EXPECT--
bool(true)
T
