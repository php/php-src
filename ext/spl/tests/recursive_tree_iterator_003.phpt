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
===DONE===
--EXPECT--
RecursiveCachingIterator::__construct() expects parameter 1 to be RecursiveIterator, object given
===DONE===
