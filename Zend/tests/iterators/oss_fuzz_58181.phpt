--TEST--
oss-fuzz #58181: Fix unexpected reference returned from CallbackFilterIterator::accept()
--FILE--
<?php
function test(array $data) {
    $iterator = new ArrayIterator($data);
    $iterator = new \CallbackFilterIterator($iterator, fn&() => true);
    $iterator->rewind();
}

test(['a', 'b']);
?>
--EXPECTF--
Notice: Only variable references should be returned by reference in %s on line %d
