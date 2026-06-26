--TEST--
Generic syntax: bound is a union type
--FILE--
<?php
class Box<T : int|string> {}
$p = (new ReflectionClass('Box'))->getGenericParameters()[0];
$b = $p->getBound();
echo get_class($b), "\n";
?>
--EXPECT--
ReflectionUnionType
