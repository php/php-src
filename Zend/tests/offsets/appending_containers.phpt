--TEST--
Appending containers
--FILE--
<?php

require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

foreach ($containers as $container) {
    echo zend_test_var_export($container), " container:\n";
    try {
        $container[] = 'value';
        var_dump($container);
    } catch (\Throwable $e) {
        echo $e->getMessage(), "\n";
    }
}

?>
--EXPECTF--
NULL container:
array(1) {
  [0]=>
  string(5) "value"
}
false container:

Deprecated: Automatic conversion of false to array is deprecated in %s on line %d
array(1) {
  [0]=>
  string(5) "value"
}
true container:
Cannot use a scalar value as an array
4 container:
Cannot use a scalar value as an array
5.5 container:
Cannot use a scalar value as an array
'10' container:
[] operator not supported for strings
'25.5' container:
[] operator not supported for strings
'string' container:
[] operator not supported for strings
[] container:
array(1) {
  [0]=>
  string(5) "value"
}
STDERR container:
Cannot use a scalar value as an array
new stdClass() container:
Cannot use object of type stdClass as array
new ArrayObject() container:
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  array(1) {
    [0]=>
    string(5) "value"
  }
}
new A() container:
string(12) "A::offsetSet"
NULL
string(5) "value"
object(A)#3 (0) {
}
new B() container:
string(12) "B::offsetSet"
NULL
string(5) "value"
object(B)#4 (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
