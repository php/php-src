--TEST--
Bug #63762 - Sigsegv when Exception::$trace is changed by user
--FILE--
<?php
$e = new Exception();

$ref = new ReflectionProperty($e, 'trace');

echo "Array of NULL:\n";
$ref->setValue($e, array(NULL));

var_dump($e->getTraceAsString());

echo "\nArray of empty array:\n";
$ref->setValue($e, array(array()));
var_dump($e->getTraceAsString());

echo "\nArray of array of NULL values:\n";
$ref->setValue($e, array(array(
    'file'  => NULL,
    'line'  => NULL,
    'class' => NULL,
    'type'  => NULL,
    'function' => NULL,
    'args'  => NULL
)));
var_dump($e->getTraceAsString());
?>
--EXPECTF--
Array of NULL:

Warning: Expected array for frame 0 in %s on line %d
string(9) "#0 {main}"

Array of empty array:
string(36) "#0 [internal function]: ()
#1 {main}"

Array of array of NULL values:

Warning: File name is not a string in %s on line %d

Warning: Value for class is not a string in %s on line %d

Warning: Value for type is not a string in %s on line %d

Warning: Value for function is not a string in %s on line %d

Warning: args element is not an array in %s on line %d
string(58) "#0 [unknown file]: [unknown][unknown][unknown]()
#1 {main}"
