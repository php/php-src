--TEST--
Reflection: composite type as generic argument
--FILE--
<?php
class Pair<A, B> {}
function f(Pair<int|string, A&B> $x): void {}
$pt = (new ReflectionFunction('f'))->getParameters()[0]->getType();
$args = $pt->getGenericArguments();
echo get_class($args[0]), "\n";
echo get_class($args[1]), "\n";
?>
--EXPECT--
ReflectionUnionType
ReflectionIntersectionType
