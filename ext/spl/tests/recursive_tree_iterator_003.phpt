--TEST--
SPL: RecursiveTreeIterator(non-traversable)
--FILE--
<?php
try {
    new RecursiveTreeIterator(new ArrayIterator(array()));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
RecursiveCachingIterator::__construct(): Argument #1 ($iterator) must be of type RecursiveIterator, ArrayIterator given
