--TEST--
Traits: trait property type
--FILE--
<?php
class Foo {}
trait Holder<T : Foo> {
    public T $val;
}
class Box {
    use Holder<Foo>;
}
$rp = (new ReflectionClass('Box'))->getProperty('val');
echo $rp->getType()->getName(), "\n";
?>
--EXPECT--
Foo
