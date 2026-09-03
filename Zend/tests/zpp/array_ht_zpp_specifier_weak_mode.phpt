--TEST--
Test array_ht ZPP specifier (weak_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

$types = require 'types.inc';
$functions = [
    'zend_array_ht',
    'zend_array_ht_or_null',
    'zend_array_ht_separate',
    'zend_array_ht_slow_zpp',
    'zend_array_ht_or_null_slow_zpp',
    'zend_array_ht_deref_separate',
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
Using zend_array_ht:

  null:            TypeError: zend_array_ht(): Argument #1 ($param) must be of type array, null given
  false:           TypeError: zend_array_ht(): Argument #1 ($param) must be of type array, false given
  true:            TypeError: zend_array_ht(): Argument #1 ($param) must be of type array, true given
  42:              TypeError: zend_array_ht(): Argument #1 ($param) must be of type array, int given
  73.5:            TypeError: zend_array_ht(): Argument #1 ($param) must be of type array, float given
  'string':        TypeError: zend_array_ht(): Argument #1 ($param) must be of type array, string given
  '15':            TypeError: zend_array_ht(): Argument #1 ($param) must be of type array, string given
  '56.7':          TypeError: zend_array_ht(): Argument #1 ($param) must be of type array, string given
  'stdClass':      TypeError: zend_array_ht(): Argument #1 ($param) must be of type array, string given
  anon class name: TypeError: zend_array_ht(): Argument #1 ($param) must be of type array, string given
  []:              array(0) {}
  new stdClass():  TypeError: zend_array_ht(): Argument #1 ($param) must be of type array, stdClass given
  new S():         TypeError: zend_array_ht(): Argument #1 ($param) must be of type array, S given
  STDOUT:          TypeError: zend_array_ht(): Argument #1 ($param) must be of type array, resource given

Using zend_array_ht_or_null:

  null:            NULL
  false:           TypeError: zend_array_ht_or_null(): Argument #1 ($param) must be of type ?array, false given
  true:            TypeError: zend_array_ht_or_null(): Argument #1 ($param) must be of type ?array, true given
  42:              TypeError: zend_array_ht_or_null(): Argument #1 ($param) must be of type ?array, int given
  73.5:            TypeError: zend_array_ht_or_null(): Argument #1 ($param) must be of type ?array, float given
  'string':        TypeError: zend_array_ht_or_null(): Argument #1 ($param) must be of type ?array, string given
  '15':            TypeError: zend_array_ht_or_null(): Argument #1 ($param) must be of type ?array, string given
  '56.7':          TypeError: zend_array_ht_or_null(): Argument #1 ($param) must be of type ?array, string given
  'stdClass':      TypeError: zend_array_ht_or_null(): Argument #1 ($param) must be of type ?array, string given
  anon class name: TypeError: zend_array_ht_or_null(): Argument #1 ($param) must be of type ?array, string given
  []:              array(0) {}
  new stdClass():  TypeError: zend_array_ht_or_null(): Argument #1 ($param) must be of type ?array, stdClass given
  new S():         TypeError: zend_array_ht_or_null(): Argument #1 ($param) must be of type ?array, S given
  STDOUT:          TypeError: zend_array_ht_or_null(): Argument #1 ($param) must be of type ?array, resource given

Using zend_array_ht_separate:

  null:            TypeError: zend_array_ht_separate(): Argument #1 ($param) must be of type array, null given
  false:           TypeError: zend_array_ht_separate(): Argument #1 ($param) must be of type array, false given
  true:            TypeError: zend_array_ht_separate(): Argument #1 ($param) must be of type array, true given
  42:              TypeError: zend_array_ht_separate(): Argument #1 ($param) must be of type array, int given
  73.5:            TypeError: zend_array_ht_separate(): Argument #1 ($param) must be of type array, float given
  'string':        TypeError: zend_array_ht_separate(): Argument #1 ($param) must be of type array, string given
  '15':            TypeError: zend_array_ht_separate(): Argument #1 ($param) must be of type array, string given
  '56.7':          TypeError: zend_array_ht_separate(): Argument #1 ($param) must be of type array, string given
  'stdClass':      TypeError: zend_array_ht_separate(): Argument #1 ($param) must be of type array, string given
  anon class name: TypeError: zend_array_ht_separate(): Argument #1 ($param) must be of type array, string given
  []:              array(0) {}
  new stdClass():  TypeError: zend_array_ht_separate(): Argument #1 ($param) must be of type array, stdClass given
  new S():         TypeError: zend_array_ht_separate(): Argument #1 ($param) must be of type array, S given
  STDOUT:          TypeError: zend_array_ht_separate(): Argument #1 ($param) must be of type array, resource given

Using zend_array_ht_slow_zpp:

  null:            TypeError: zend_array_ht_slow_zpp(): Argument #1 ($param) must be of type array, null given
  false:           TypeError: zend_array_ht_slow_zpp(): Argument #1 ($param) must be of type array, false given
  true:            TypeError: zend_array_ht_slow_zpp(): Argument #1 ($param) must be of type array, true given
  42:              TypeError: zend_array_ht_slow_zpp(): Argument #1 ($param) must be of type array, int given
  73.5:            TypeError: zend_array_ht_slow_zpp(): Argument #1 ($param) must be of type array, float given
  'string':        TypeError: zend_array_ht_slow_zpp(): Argument #1 ($param) must be of type array, string given
  '15':            TypeError: zend_array_ht_slow_zpp(): Argument #1 ($param) must be of type array, string given
  '56.7':          TypeError: zend_array_ht_slow_zpp(): Argument #1 ($param) must be of type array, string given
  'stdClass':      TypeError: zend_array_ht_slow_zpp(): Argument #1 ($param) must be of type array, string given
  anon class name: TypeError: zend_array_ht_slow_zpp(): Argument #1 ($param) must be of type array, string given
  []:              array(0) {}
  new stdClass():  TypeError: zend_array_ht_slow_zpp(): Argument #1 ($param) must be of type array, stdClass given
  new S():         TypeError: zend_array_ht_slow_zpp(): Argument #1 ($param) must be of type array, S given
  STDOUT:          TypeError: zend_array_ht_slow_zpp(): Argument #1 ($param) must be of type array, resource given

Using zend_array_ht_or_null_slow_zpp:

  null:            NULL
  false:           TypeError: zend_array_ht_or_null_slow_zpp(): Argument #1 ($param) must be of type ?array, false given
  true:            TypeError: zend_array_ht_or_null_slow_zpp(): Argument #1 ($param) must be of type ?array, true given
  42:              TypeError: zend_array_ht_or_null_slow_zpp(): Argument #1 ($param) must be of type ?array, int given
  73.5:            TypeError: zend_array_ht_or_null_slow_zpp(): Argument #1 ($param) must be of type ?array, float given
  'string':        TypeError: zend_array_ht_or_null_slow_zpp(): Argument #1 ($param) must be of type ?array, string given
  '15':            TypeError: zend_array_ht_or_null_slow_zpp(): Argument #1 ($param) must be of type ?array, string given
  '56.7':          TypeError: zend_array_ht_or_null_slow_zpp(): Argument #1 ($param) must be of type ?array, string given
  'stdClass':      TypeError: zend_array_ht_or_null_slow_zpp(): Argument #1 ($param) must be of type ?array, string given
  anon class name: TypeError: zend_array_ht_or_null_slow_zpp(): Argument #1 ($param) must be of type ?array, string given
  []:              array(0) {}
  new stdClass():  TypeError: zend_array_ht_or_null_slow_zpp(): Argument #1 ($param) must be of type ?array, stdClass given
  new S():         TypeError: zend_array_ht_or_null_slow_zpp(): Argument #1 ($param) must be of type ?array, S given
  STDOUT:          TypeError: zend_array_ht_or_null_slow_zpp(): Argument #1 ($param) must be of type ?array, resource given

Using zend_array_ht_deref_separate:

  null:            TypeError: zend_array_ht_deref_separate(): Argument #1 ($param) must be of type array, null given
  false:           TypeError: zend_array_ht_deref_separate(): Argument #1 ($param) must be of type array, false given
  true:            TypeError: zend_array_ht_deref_separate(): Argument #1 ($param) must be of type array, true given
  42:              TypeError: zend_array_ht_deref_separate(): Argument #1 ($param) must be of type array, int given
  73.5:            TypeError: zend_array_ht_deref_separate(): Argument #1 ($param) must be of type array, float given
  'string':        TypeError: zend_array_ht_deref_separate(): Argument #1 ($param) must be of type array, string given
  '15':            TypeError: zend_array_ht_deref_separate(): Argument #1 ($param) must be of type array, string given
  '56.7':          TypeError: zend_array_ht_deref_separate(): Argument #1 ($param) must be of type array, string given
  'stdClass':      TypeError: zend_array_ht_deref_separate(): Argument #1 ($param) must be of type array, string given
  anon class name: TypeError: zend_array_ht_deref_separate(): Argument #1 ($param) must be of type array, string given
  []:              array(0) {}
  new stdClass():  TypeError: zend_array_ht_deref_separate(): Argument #1 ($param) must be of type array, stdClass given
  new S():         TypeError: zend_array_ht_deref_separate(): Argument #1 ($param) must be of type array, S given
  STDOUT:          TypeError: zend_array_ht_deref_separate(): Argument #1 ($param) must be of type array, resource given
