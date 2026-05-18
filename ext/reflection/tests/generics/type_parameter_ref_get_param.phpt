--TEST--
Reflection: ReflectionTypeParameterReference::getTypeParameter()
--FILE--
<?php
class Box<X> {}
class Outer<T : object> {
    public Box<T> $b;
}
$rt = (new ReflectionClass('Outer'))->getProperty('b')->getType();
$ref = $rt->getGenericArguments()[0];
$param = $ref->getTypeParameter();
echo get_class($param), "\n";
echo $param->getName(), "\n";
echo $param->getDeclaringEntity()->getName(), "\n";
?>
--EXPECT--
ReflectionGenericTypeParameter
T
Outer
