--TEST--
Reflection: nested generic arguments
--FILE--
<?php
class Box<A> {}
function f(Box<Box<int>> $x): void {}
$pt = (new ReflectionFunction('f'))->getParameters()[0]->getType();
echo $pt->getName(), "\n";
$inner = $pt->getGenericArguments()[0];
echo $inner->getName(), "\n";
$inmost = $inner->getGenericArguments()[0];
echo $inmost->getName(), "\n";
?>
--EXPECT--
Box
Box
int
