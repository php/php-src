--TEST--
SPL: RecursiveTreeIterator(non-traversable)
--FILE--
<?php
try {
    new RecursiveTreeIterator(new ArrayIterator(array()));
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: RecursiveCachingIterator::__construct(): Argument #1 ($iterator) must be of type RecursiveIterator, ArrayIterator given
