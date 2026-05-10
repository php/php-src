--TEST--
Erasure: generic property type erased to bound
--FILE--
<?php
class Foo {}
class Box<T : Foo> {
    public T $value;
}
$rp = (new ReflectionClass('Box'))->getProperty('value');
$rt = $rp->getType();
echo get_class($rt), "\n";
echo $rt->getName(), "\n";
?>
--EXPECT--
ReflectionNamedType
Foo
