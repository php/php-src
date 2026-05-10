--TEST--
Generic syntax: bound is an intersection type
--FILE--
<?php
interface A {}
interface B {}
class Box<T : A&B> {}
$p = (new ReflectionClass('Box'))->getGenericParameters()[0];
echo get_class($p->getBound()), "\n";
?>
--EXPECT--
ReflectionIntersectionType
