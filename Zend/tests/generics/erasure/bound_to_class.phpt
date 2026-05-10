--TEST--
Erasure: T : Foo erases to Foo
--FILE--
<?php
class Foo {}
function id<T : Foo>(T $x): T { return $x; }
$r = new ReflectionFunction('id');
echo $r->getParameters()[0]->getType()->getName(), "\n";
echo $r->getReturnType()->getName(), "\n";
?>
--EXPECT--
Foo
Foo
