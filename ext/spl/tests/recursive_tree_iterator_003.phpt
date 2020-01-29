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
RecursiveCachingIterator::__construct() expects argument #1 ($iterator) to be of type RecursiveIterator, object given
