--TEST--
Trait method aliasing and interface methods are matched case-sensitively
--FILE--
<?php

trait Iter {
    public function iter(): Iterator {
        return new ArrayIterator([1, 2, 3]);
    }
}

class Good implements IteratorAggregate {
    use Iter { iter as getIterator; }
}

foreach (new Good() as $value) {
    var_dump($value);
}

class Bad implements IteratorAggregate {
    use Iter { iter as getiterator; }
}

?>
--EXPECTF--
int(1)
int(2)
int(3)

Fatal error: Class Bad contains 1 abstract method and must therefore be declared abstract or implement the remaining method (IteratorAggregate::getIterator) in %s on line %d
