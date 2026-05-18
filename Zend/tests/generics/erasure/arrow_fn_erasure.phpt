--TEST--
Erasure: arrow function with type parameters erases to bound
--FILE--
<?php
$f = fn<T : int>(T $x): T => $x;
$r = new ReflectionFunction($f);
echo $r->getParameters()[0]->getType()->__toString(), "\n";
?>
--EXPECT--
int
