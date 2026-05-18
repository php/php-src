--TEST--
Reflection: ReflectionTypeParameterReference appears inside getGenericArguments
--FILE--
<?php
class Box<X> {}
class Outer<T : object> {
    public Box<T> $b;
}
$rp = (new ReflectionClass('Outer'))->getProperty('b');
$rt = $rp->getType();
echo $rt->getName(), "\n";
$args = $rt->getGenericArguments();
echo count($args), "\n";
echo get_class($args[0]), "\n";
echo $args[0]->getName(), "\n";
?>
--EXPECT--
Box
1
ReflectionTypeParameterReference
T
