--TEST--
Generic syntax: type argument is DNF
--FILE--
<?php
interface A {}
interface B {}
class C {}
class D {}
function f(D<(A&B)|C> $x): void {}
$pt = (new ReflectionFunction('f'))->getParameters()[0]->getType();
$arg = $pt->getGenericArguments()[0];
echo get_class($arg), "\n";
?>
--EXPECT--
ReflectionUnionType
