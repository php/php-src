--TEST--
Appending containers via a read operation $c[][5] = $v;
--FILE--
<?php

require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

foreach ($containers as $container) {
    try {
        $container[][5] = 'value';
        var_dump($container);
    } catch (\Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

?>
--EXPECTF--
array(1) {
  [0]=>
  array(1) {
    [5]=>
    string(5) "value"
  }
}

Deprecated: Automatic conversion of false to array is deprecated in %s on line %d
array(1) {
  [0]=>
  array(1) {
    [5]=>
    string(5) "value"
  }
}
Error: Cannot use a scalar value as an array
Error: Cannot use a scalar value as an array
Error: Cannot use a scalar value as an array
Error: [] operator not supported for strings
Error: [] operator not supported for strings
Error: [] operator not supported for strings
array(1) {
  [0]=>
  array(1) {
    [5]=>
    string(5) "value"
  }
}
Error: Cannot use a scalar value as an array
Error: Cannot use object of type stdClass as array

Notice: Indirect modification of overloaded element of ArrayObject has no effect in %s on line %d
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
string(12) "A::offsetGet"
NULL

Notice: Indirect modification of overloaded element of A has no effect in %s on line %d

Warning: Only the first byte will be assigned to the string offset in %s on line %d
object(A)#3 (0) {
}

Notice: Indirect modification of overloaded element of B has no effect in %s on line %d
ArgumentCountError: B::offsetGet(): Argument #1 ($offset) not passed
