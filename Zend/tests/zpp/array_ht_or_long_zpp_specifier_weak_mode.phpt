--TEST--
Test array_ht_or_long ZPP specifier (weak_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

$types = require 'types.inc';
$functions = [
    'zend_array_ht_or_long',
    'zend_array_ht_or_long_or_null',
];

foreach ($functions as $i => $function) {
    echo "Using $function:\n\n";

    foreach ($types as $name => $type) {
        printf("  %-16s ", "$name:");

        try {
            $result = $function($type);

            $result === []
                ? printf("array(0) {}\n")
                : var_dump($result);

        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
    }

    if ($i !== array_key_last($functions)) {
        echo "\n";
    }
}

?>
--EXPECTF--
Using zend_array_ht_or_long:

  null:            
Deprecated: zend_array_ht_or_long(): Passing null to parameter #1 ($param) of type array|int is deprecated in %s on line %d
int(0)
  false:           int(0)
  true:            int(1)
  42:              int(42)
  73.5:            
Deprecated: Implicit conversion from float 73.5 to int loses precision in %s on line %d
int(73)
  'string':        TypeError: zend_array_ht_or_long(): Argument #1 ($param) must be of type array|int, string given
  '15':            int(15)
  '56.7':          
Deprecated: Implicit conversion from float-string "56.7" to int loses precision in %s on line %d
int(56)
  'stdClass':      TypeError: zend_array_ht_or_long(): Argument #1 ($param) must be of type array|int, string given
  anon class name: TypeError: zend_array_ht_or_long(): Argument #1 ($param) must be of type array|int, string given
  []:              array(0) {}
  new stdClass():  TypeError: zend_array_ht_or_long(): Argument #1 ($param) must be of type array|int, stdClass given
  new S():         TypeError: zend_array_ht_or_long(): Argument #1 ($param) must be of type array|int, S given
  STDOUT:          TypeError: zend_array_ht_or_long(): Argument #1 ($param) must be of type array|int, resource given

Using zend_array_ht_or_long_or_null:

  null:            NULL
  false:           int(0)
  true:            int(1)
  42:              int(42)
  73.5:            
Deprecated: Implicit conversion from float 73.5 to int loses precision in %s on line %d
int(73)
  'string':        TypeError: zend_array_ht_or_long_or_null(): Argument #1 ($param) must be of type array|int|null, string given
  '15':            int(15)
  '56.7':          
Deprecated: Implicit conversion from float-string "56.7" to int loses precision in %s on line %d
int(56)
  'stdClass':      TypeError: zend_array_ht_or_long_or_null(): Argument #1 ($param) must be of type array|int|null, string given
  anon class name: TypeError: zend_array_ht_or_long_or_null(): Argument #1 ($param) must be of type array|int|null, string given
  []:              array(0) {}
  new stdClass():  TypeError: zend_array_ht_or_long_or_null(): Argument #1 ($param) must be of type array|int|null, stdClass given
  new S():         TypeError: zend_array_ht_or_long_or_null(): Argument #1 ($param) must be of type array|int|null, S given
  STDOUT:          TypeError: zend_array_ht_or_long_or_null(): Argument #1 ($param) must be of type array|int|null, resource given
