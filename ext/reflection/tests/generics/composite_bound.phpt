--TEST--
Reflection: composite (union) bound returned
--FILE--
<?php
class A {}
function f<T : A|int>(T $x): T { return $x; }
$p = (new ReflectionFunction('f'))->getGenericParameters()[0];
$b = $p->getBound();
echo get_class($b), "\n";
?>
--EXPECT--
ReflectionUnionType
