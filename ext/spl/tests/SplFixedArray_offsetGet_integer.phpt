--TEST--
Using SplFixedArray::offsetGet() with various types
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--FILE--
<?php
$arr = new SplFixedArray(2);

$arr[0] = 'foo';
$arr[1] = 'bar';

var_dump($arr->offsetGet(0.2));
var_dump($arr->offsetGet(false));
var_dump($arr->offsetGet(true));
?>
--EXPECT--
string(3) "foo"
string(3) "foo"
string(3) "bar"
