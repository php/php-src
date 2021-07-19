--TEST--
RecursiveIteratorIterator constructor should thrown if IteratorAggregate does not return Iterator
--FILE--
<?php

class MyIteratorAggregate implements IteratorAggregate {
    #[ReturnTypeWillChange]
    function getIterator() {
        return null;
    }
}

try {
    new RecursiveIteratorIterator(new MyIteratorAggregate);
} catch (LogicException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
MyIteratorAggregate::getIterator() must return an object that implements Traversable
