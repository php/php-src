--TEST--
Erasure: unbounded T erases to mixed
--FILE--
<?php
function id<T>(T $x): T { return $x; }
$r = new ReflectionFunction('id');
echo $r->getParameters()[0]->getType()->__toString(), "\n";
echo $r->getReturnType()->__toString(), "\n";
?>
--EXPECT--
mixed
mixed
