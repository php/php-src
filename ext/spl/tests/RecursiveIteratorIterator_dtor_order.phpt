--TEST--
Handle object_iterator being destroyed before the RecursiveIteratorIterator object
--FILE--
<?php

$dummy = new stdClass; // Dummy object to control object store ordering
$it = new RecursiveIteratorIterator(new RecursiveArrayIterator([1]));
unset($dummy);
foreach ($it as $v) {
    eval('class self {}'); // Cause a bailout.
}
?>
--EXPECTF--
Fatal error: Cannot use 'self' as class name as it is reserved in %s on line %d
