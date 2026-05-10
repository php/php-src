--TEST--
Generic syntax: trailing comma in type argument list
--FILE--
<?php
class C {}
function f(C<int, string,> $x): void {}
$pt = (new ReflectionFunction('f'))->getParameters()[0]->getType();
echo count($pt->getGenericArguments()), "\n";
?>
--EXPECT--
2
