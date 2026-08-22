--TEST--
Test array_or_object_ht ZPP specifier (strict_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

declare(strict_types=1);

$types = require 'types.inc';
$functions = [
    'zend_array_or_object_ht',
    'zend_array_or_object_ht_separate',
    'zend_array_or_object_ht_slow_zpp',
    'zend_array_or_object_ht_deref_separate',
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
Using zend_array_or_object_ht:

  null:            TypeError: zend_array_or_object_ht(): Argument #1 ($param) must be of type array, null given
  false:           TypeError: zend_array_or_object_ht(): Argument #1 ($param) must be of type array, false given
  true:            TypeError: zend_array_or_object_ht(): Argument #1 ($param) must be of type array, true given
  42:              TypeError: zend_array_or_object_ht(): Argument #1 ($param) must be of type array, int given
  73.5:            TypeError: zend_array_or_object_ht(): Argument #1 ($param) must be of type array, float given
  'string':        TypeError: zend_array_or_object_ht(): Argument #1 ($param) must be of type array, string given
  '15':            TypeError: zend_array_or_object_ht(): Argument #1 ($param) must be of type array, string given
  '56.7':          TypeError: zend_array_or_object_ht(): Argument #1 ($param) must be of type array, string given
  'stdClass':      TypeError: zend_array_or_object_ht(): Argument #1 ($param) must be of type array, string given
  anon class name: TypeError: zend_array_or_object_ht(): Argument #1 ($param) must be of type array, string given
  []:              array(0) {}
  new stdClass():  array(0) {}
  new S():         array(0) {}
  STDOUT:          TypeError: zend_array_or_object_ht(): Argument #1 ($param) must be of type array, resource given

Using zend_array_or_object_ht_separate:

  null:            TypeError: zend_array_or_object_ht_separate(): Argument #1 ($param) must be of type array, null given
  false:           TypeError: zend_array_or_object_ht_separate(): Argument #1 ($param) must be of type array, false given
  true:            TypeError: zend_array_or_object_ht_separate(): Argument #1 ($param) must be of type array, true given
  42:              TypeError: zend_array_or_object_ht_separate(): Argument #1 ($param) must be of type array, int given
  73.5:            TypeError: zend_array_or_object_ht_separate(): Argument #1 ($param) must be of type array, float given
  'string':        TypeError: zend_array_or_object_ht_separate(): Argument #1 ($param) must be of type array, string given
  '15':            TypeError: zend_array_or_object_ht_separate(): Argument #1 ($param) must be of type array, string given
  '56.7':          TypeError: zend_array_or_object_ht_separate(): Argument #1 ($param) must be of type array, string given
  'stdClass':      TypeError: zend_array_or_object_ht_separate(): Argument #1 ($param) must be of type array, string given
  anon class name: TypeError: zend_array_or_object_ht_separate(): Argument #1 ($param) must be of type array, string given
  []:              array(0) {}
  new stdClass():  array(0) {}
  new S():         array(0) {}
  STDOUT:          TypeError: zend_array_or_object_ht_separate(): Argument #1 ($param) must be of type array, resource given

Using zend_array_or_object_ht_slow_zpp:

  null:            TypeError: zend_array_or_object_ht_slow_zpp(): Argument #1 ($param) must be of type array, null given
  false:           TypeError: zend_array_or_object_ht_slow_zpp(): Argument #1 ($param) must be of type array, false given
  true:            TypeError: zend_array_or_object_ht_slow_zpp(): Argument #1 ($param) must be of type array, true given
  42:              TypeError: zend_array_or_object_ht_slow_zpp(): Argument #1 ($param) must be of type array, int given
  73.5:            TypeError: zend_array_or_object_ht_slow_zpp(): Argument #1 ($param) must be of type array, float given
  'string':        TypeError: zend_array_or_object_ht_slow_zpp(): Argument #1 ($param) must be of type array, string given
  '15':            TypeError: zend_array_or_object_ht_slow_zpp(): Argument #1 ($param) must be of type array, string given
  '56.7':          TypeError: zend_array_or_object_ht_slow_zpp(): Argument #1 ($param) must be of type array, string given
  'stdClass':      TypeError: zend_array_or_object_ht_slow_zpp(): Argument #1 ($param) must be of type array, string given
  anon class name: TypeError: zend_array_or_object_ht_slow_zpp(): Argument #1 ($param) must be of type array, string given
  []:              array(0) {}
  new stdClass():  array(0) {}
  new S():         array(0) {}
  STDOUT:          TypeError: zend_array_or_object_ht_slow_zpp(): Argument #1 ($param) must be of type array, resource given

Using zend_array_or_object_ht_deref_separate:

  null:            TypeError: zend_array_or_object_ht_deref_separate(): Argument #1 ($param) must be of type array, null given
  false:           TypeError: zend_array_or_object_ht_deref_separate(): Argument #1 ($param) must be of type array, false given
  true:            TypeError: zend_array_or_object_ht_deref_separate(): Argument #1 ($param) must be of type array, true given
  42:              TypeError: zend_array_or_object_ht_deref_separate(): Argument #1 ($param) must be of type array, int given
  73.5:            TypeError: zend_array_or_object_ht_deref_separate(): Argument #1 ($param) must be of type array, float given
  'string':        TypeError: zend_array_or_object_ht_deref_separate(): Argument #1 ($param) must be of type array, string given
  '15':            TypeError: zend_array_or_object_ht_deref_separate(): Argument #1 ($param) must be of type array, string given
  '56.7':          TypeError: zend_array_or_object_ht_deref_separate(): Argument #1 ($param) must be of type array, string given
  'stdClass':      TypeError: zend_array_or_object_ht_deref_separate(): Argument #1 ($param) must be of type array, string given
  anon class name: TypeError: zend_array_or_object_ht_deref_separate(): Argument #1 ($param) must be of type array, string given
  []:              array(0) {}
  new stdClass():  array(0) {}
  new S():         array(0) {}
  STDOUT:          TypeError: zend_array_or_object_ht_deref_separate(): Argument #1 ($param) must be of type array, resource given
