--TEST--
SplFixedArray::offsetExists with various types
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--FILE--
<?php var_dump((new SplFixedArray(1))->offsetExists(0.1)); ?>
--EXPECT--
bool(false)
