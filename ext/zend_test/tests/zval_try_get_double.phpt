--TEST--
zval_try_get_double() conversion semantics
--EXTENSIONS--
zend_test
--FILE--
<?php

foreach ([
    null,
    false,
    true,
    42,
    42.5,
    "42",
    "42.5",
    "1e3",
    "not numeric",
    [],
    new FloatCastableNoOperations(42.5),
    new LongCastableNoOperations(42),
] as $value) {
    var_dump(zend_test_zval_try_get_double($value));
}

$resource = fopen(__FILE__, 'r');
var_dump(zend_test_zval_try_get_double($resource));

var_dump(zend_test_zval_try_get_double("42 with trailing data"));

set_error_handler(static function (int $errno, string $errstr): never {
    throw new Exception($errstr);
});
try {
    zend_test_zval_try_get_double("42 with trailing data");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
array(2) {
  ["value"]=>
  float(0)
  ["failed"]=>
  bool(false)
}
array(2) {
  ["value"]=>
  float(0)
  ["failed"]=>
  bool(false)
}
array(2) {
  ["value"]=>
  float(1)
  ["failed"]=>
  bool(false)
}
array(2) {
  ["value"]=>
  float(42)
  ["failed"]=>
  bool(false)
}
array(2) {
  ["value"]=>
  float(42.5)
  ["failed"]=>
  bool(false)
}
array(2) {
  ["value"]=>
  float(42)
  ["failed"]=>
  bool(false)
}
array(2) {
  ["value"]=>
  float(42.5)
  ["failed"]=>
  bool(false)
}
array(2) {
  ["value"]=>
  float(1000)
  ["failed"]=>
  bool(false)
}
array(2) {
  ["value"]=>
  float(0)
  ["failed"]=>
  bool(true)
}
array(2) {
  ["value"]=>
  float(0)
  ["failed"]=>
  bool(true)
}
array(2) {
  ["value"]=>
  float(42.5)
  ["failed"]=>
  bool(false)
}
array(2) {
  ["value"]=>
  float(0)
  ["failed"]=>
  bool(true)
}
array(2) {
  ["value"]=>
  float(0)
  ["failed"]=>
  bool(true)
}

Warning: A non-numeric value encountered in %s on line %d
array(2) {
  ["value"]=>
  float(42)
  ["failed"]=>
  bool(false)
}
Exception: A non-numeric value encountered
