--TEST--
Appending containers via a fetch operation $c[][5] = $v;
--FILE--
<?php

require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

foreach ($containers as $container) {
    echo zend_test_var_export($container), " container:\n";
    try {
        $container[][5] = 'value';
        var_dump($container);
    } catch (\Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

?>
--EXPECTF--
NULL container:
array(1) {
  [0]=>
  array(1) {
    [5]=>
    string(5) "value"
  }
}
false container:

Deprecated: Automatic conversion of false to array is deprecated in %s on line %d
array(1) {
  [0]=>
  array(1) {
    [5]=>
    string(5) "value"
  }
}
true container:
Error: Cannot use a scalar value as an array
4 container:
Error: Cannot use a scalar value as an array
5.5 container:
Error: Cannot use a scalar value as an array
'10' container:
Error: [] operator not supported for strings
'25.5' container:
Error: [] operator not supported for strings
'string' container:
Error: [] operator not supported for strings
[] container:
array(1) {
  [0]=>
  array(1) {
    [5]=>
    string(5) "value"
  }
}
STDERR container:
Error: Cannot use a scalar value as an array
new stdClass() container:
Error: Cannot use object of type stdClass as array
new ArrayObject() container:

Notice: Indirect modification of overloaded element of ArrayObject has no effect in %s on line %d
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
new A() container:
string(12) "A::offsetGet"
NULL

Notice: Indirect modification of overloaded element of A has no effect in %s on line %d
Error: Cannot use a scalar value as an array
new B() container:

Notice: Indirect modification of overloaded element of B has no effect in %s on line %d
ArgumentCountError: B::offsetGet(): Argument #1 ($offset) not passed
