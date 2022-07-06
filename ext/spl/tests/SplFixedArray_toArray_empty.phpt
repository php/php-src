--TEST--
SplFixedArray::toArray with empty array
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--FILE--
<?php var_dump((new SplFixedArray())->toArray()); ?>
--EXPECT--
array(0) {
}
