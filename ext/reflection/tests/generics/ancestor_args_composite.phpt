--TEST--
Reflection: composite (union/intersection) type args inside extends/implements
--FILE--
<?php
class A {}
class B {}
class Pair<X, Y> {}

class C extends Pair<A|B, A&B> {}

$args = (new ReflectionClass('C'))->getGenericArgumentsForParentClass();
echo "count: ", count($args), "\n";
echo "[0] kind: ", get_class($args[0]), "\n";
echo "[1] kind: ", get_class($args[1]), "\n";
?>
--EXPECT--
count: 2
[0] kind: ReflectionUnionType
[1] kind: ReflectionIntersectionType
