--TEST--
Appending containers
--FILE--
<?php

require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

foreach ($containers as $container) {
    try {
        $container[] = 'value';
        var_dump($container);
    } catch (\Throwable $e) {
        echo $e->getMessage(), "\n";
    }
}

?>
--EXPECTF--
array(1) {
  [0]=>
  string(5) "value"
}

Deprecated: Automatic conversion of false to array is deprecated in %s on line %d
array(1) {
  [0]=>
  string(5) "value"
}
Cannot use a scalar value as an array
Cannot use a scalar value as an array
Cannot use a scalar value as an array
[] operator not supported for strings
[] operator not supported for strings
[] operator not supported for strings
array(1) {
  [0]=>
  string(5) "value"
}
Cannot use a scalar value as an array
Cannot use object of type stdClass as array
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  array(1) {
    [0]=>
    string(5) "value"
  }
}
string(12) "A::offsetSet"
NULL
string(5) "value"
object(A)#3 (0) {
}
string(12) "B::offsetSet"
NULL
string(5) "value"
object(B)#4 (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
