--TEST--
Erasure: closure with type parameters erases to bound
--FILE--
<?php
$f = function <T : object>(T $x): T { return $x; };
$r = new ReflectionFunction($f);
echo $r->getParameters()[0]->getType()->__toString(), "\n";
?>
--EXPECT--
object
