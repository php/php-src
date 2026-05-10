--TEST--
Generic syntax: bound is a DNF type
--FILE--
<?php
interface A {}
interface B {}
class C {}
class Box<T : (A&B)|C> {}
$p = (new ReflectionClass('Box'))->getGenericParameters()[0];
echo get_class($p->getBound()), "\n";
?>
--EXPECT--
ReflectionUnionType
