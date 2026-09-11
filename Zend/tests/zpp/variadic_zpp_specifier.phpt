--TEST--
Test variadic ZPP specifiers
--EXTENSIONS--
zend_test
--FILE--
<?php

var_dump(zend_variadic());
var_dump(zend_variadic(1, "foo", [1, 2]));
var_dump(zend_variadic_slow_zpp());
var_dump(zend_variadic_slow_zpp(1, "foo", [1, 2]));
var_dump(zend_variadic_with_named(1, "foo", b: "bar"));

try {
    zend_variadic(1, b: "bar");
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
array(0) {
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  string(3) "foo"
  [2]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}
array(0) {
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  string(3) "foo"
  [2]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  string(3) "foo"
  ["b"]=>
  string(3) "bar"
}
ArgumentCountError: Internal function zend_variadic() does not accept named variadic arguments
