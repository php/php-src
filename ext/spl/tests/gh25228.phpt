--TEST--
GH-25228 (Segfault in IteratorIterator::__construct() when $class is an abstract IteratorAggregate)
--FILE--
<?php

abstract class A implements IteratorAggregate {}
class B extends A {
    public function getIterator(): Iterator {
        return new ArrayIterator([1]);
    }
}

abstract class C implements IteratorAggregate {
    abstract public function getIterator(): Iterator;
}
class D extends C {
    public function getIterator(): Iterator {
        return new ArrayIterator([1]);
    }
}

try {
    new IteratorIterator(new B, 'A');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    new IteratorIterator(new D, 'C');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

foreach (new IteratorIterator(new B, 'B') as $v) {
    var_dump($v);
}

?>
--EXPECT--
Cannot call abstract method IteratorAggregate::getIterator()
Cannot call abstract method C::getIterator()
int(1)
