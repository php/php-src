--TEST--
Generic syntax: type argument is an intersection
--FILE--
<?php
interface A {}
interface B {}
class C {}
function f(C<A&B> $x): void {}
$pt = (new ReflectionFunction('f'))->getParameters()[0]->getType();
$arg = $pt->getGenericArguments()[0];
echo get_class($arg), "\n";
?>
--EXPECT--
ReflectionIntersectionType
