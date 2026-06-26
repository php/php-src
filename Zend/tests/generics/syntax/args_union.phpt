--TEST--
Generic syntax: type argument is a union
--FILE--
<?php
class Container {}
function f(Container<int|string|null> $x): void {}
$pt = (new ReflectionFunction('f'))->getParameters()[0]->getType();
$arg = $pt->getGenericArguments()[0];
echo get_class($arg), "\n";
?>
--EXPECT--
ReflectionUnionType
