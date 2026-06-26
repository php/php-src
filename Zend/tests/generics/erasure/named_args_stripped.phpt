--TEST--
Erasure: type arguments stripped from named types in runtime view
--FILE--
<?php
class Container {}
function f(Container<int> $x): Container<string> { return $x; }
$r = new ReflectionFunction('f');
echo $r->getParameters()[0]->getType()->getName(), "\n";
echo $r->getReturnType()->getName(), "\n";
?>
--EXPECT--
Container
Container
