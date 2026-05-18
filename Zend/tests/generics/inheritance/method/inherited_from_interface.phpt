--TEST--
Inherited methods: abstract method inherited from a generic interface is substituted on the implementing class
--FILE--
<?php
interface Box<T> {
    public function unwrap(): T;
}

abstract class IntBoxImpl implements Box<int> {}

$rm = (new ReflectionClass('IntBoxImpl'))->getMethod('unwrap');
echo $rm->getReturnType()->getName(), "\n";
?>
--EXPECT--
int
