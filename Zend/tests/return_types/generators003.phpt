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
echo get_class($some->getIterator());

?>
--EXPECT--
Generator
