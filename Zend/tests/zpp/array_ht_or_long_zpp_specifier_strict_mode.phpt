--TEST--
Test array_ht_or_long ZPP specifier (strict_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

declare(strict_types=1);

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
--EXPECT--
Using zend_array_ht_or_long:

  null:            TypeError: zend_array_ht_or_long(): Argument #1 ($param) must be of type array|int, null given
  false:           TypeError: zend_array_ht_or_long(): Argument #1 ($param) must be of type array|int, false given
  true:            TypeError: zend_array_ht_or_long(): Argument #1 ($param) must be of type array|int, true given
  42:              int(42)
  73.5:            TypeError: zend_array_ht_or_long(): Argument #1 ($param) must be of type array|int, float given
  'string':        TypeError: zend_array_ht_or_long(): Argument #1 ($param) must be of type array|int, string given
  '15':            TypeError: zend_array_ht_or_long(): Argument #1 ($param) must be of type array|int, string given
  '56.7':          TypeError: zend_array_ht_or_long(): Argument #1 ($param) must be of type array|int, string given
  'stdClass':      TypeError: zend_array_ht_or_long(): Argument #1 ($param) must be of type array|int, string given
  anon class name: TypeError: zend_array_ht_or_long(): Argument #1 ($param) must be of type array|int, string given
  []:              array(0) {}
  new stdClass():  TypeError: zend_array_ht_or_long(): Argument #1 ($param) must be of type array|int, stdClass given
  new S():         TypeError: zend_array_ht_or_long(): Argument #1 ($param) must be of type array|int, S given
  STDOUT:          TypeError: zend_array_ht_or_long(): Argument #1 ($param) must be of type array|int, resource given

Using zend_array_ht_or_long_or_null:

  null:            NULL
  false:           TypeError: zend_array_ht_or_long_or_null(): Argument #1 ($param) must be of type array|int|null, false given
  true:            TypeError: zend_array_ht_or_long_or_null(): Argument #1 ($param) must be of type array|int|null, true given
  42:              int(42)
  73.5:            TypeError: zend_array_ht_or_long_or_null(): Argument #1 ($param) must be of type array|int|null, float given
  'string':        TypeError: zend_array_ht_or_long_or_null(): Argument #1 ($param) must be of type array|int|null, string given
  '15':            TypeError: zend_array_ht_or_long_or_null(): Argument #1 ($param) must be of type array|int|null, string given
  '56.7':          TypeError: zend_array_ht_or_long_or_null(): Argument #1 ($param) must be of type array|int|null, string given
  'stdClass':      TypeError: zend_array_ht_or_long_or_null(): Argument #1 ($param) must be of type array|int|null, string given
  anon class name: TypeError: zend_array_ht_or_long_or_null(): Argument #1 ($param) must be of type array|int|null, string given
  []:              array(0) {}
  new stdClass():  TypeError: zend_array_ht_or_long_or_null(): Argument #1 ($param) must be of type array|int|null, stdClass given
  new S():         TypeError: zend_array_ht_or_long_or_null(): Argument #1 ($param) must be of type array|int|null, S given
  STDOUT:          TypeError: zend_array_ht_or_long_or_null(): Argument #1 ($param) must be of type array|int|null, resource given
