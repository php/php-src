--TEST--
Erasure: T : object erases to object
--FILE--
<?php
function id<T : object>(T $x): T { return $x; }
$r = new ReflectionFunction('id');
echo $r->getParameters()[0]->getType()->__toString(), "\n";
echo $r->getReturnType()->__toString(), "\n";
?>
--EXPECT--
object
object
