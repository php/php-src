--TEST--
Bug #60161: Implementing an interface extending Traversable is order dependent
--FILE--
<?php

interface Foo extends Traversable {
}

class Example implements Foo, IteratorAggregate {
    public function getIterator() {
        return new ArrayIterator([]);
    }
}

?>
===DONE===
--EXPECT--
===DONE===
