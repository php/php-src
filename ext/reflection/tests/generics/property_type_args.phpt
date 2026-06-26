--TEST--
Reflection: property type arguments via Reflection
--FILE--
<?php
class Box<T> {}
class Holder {
    public Box<int> $b;
}
$rt = (new ReflectionClass('Holder'))->getProperty('b')->getType();
echo $rt->getName(), "\n";
echo count($rt->getGenericArguments()), "\n";
echo $rt->getGenericArguments()[0]->getName(), "\n";
?>
--EXPECT--
Box
1
int
