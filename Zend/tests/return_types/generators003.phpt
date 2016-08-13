--TEST--
Return type covariance works with generators

--FILE--
<?php
interface Collection extends IteratorAggregate {
    function getIterator(): Iterator;
}

class SomeCollection implements Collection {
    function getIterator(): Generator {
        foreach ($this->data as $key => $value) {
            yield $key => $value;
        }
    }
}

$some = new SomeCollection();
var_dump($some->getIterator());

--EXPECTF--
Fatal error: Declaration of SomeCollection::getIterator(): Generator must be compatible with Collection::getIterator(): Iterator in %sgenerators003.php on line 6
