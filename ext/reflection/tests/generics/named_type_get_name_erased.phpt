--TEST--
Reflection: ReflectionNamedType::getName() returns the erased name
--FILE--
<?php
class Box<T> {}
function f(Box<int> $x): Box<string> { return $x; }
$r = new ReflectionFunction('f');
echo $r->getParameters()[0]->getType()->getName(), "\n";
echo $r->getReturnType()->getName(), "\n";
?>
--EXPECT--
Box
Box
