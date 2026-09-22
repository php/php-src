--TEST--
Test array_ht_or_str ZPP specifier (strict_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

declare(strict_types=1);

$types = require 'types.inc';
$functions = [
    'zend_array_ht_or_str',
    'zend_array_ht_or_str_or_null',
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
Using zend_array_ht_or_str:

  null:            TypeError: zend_array_ht_or_str(): Argument #1 ($param) must be of type array|string, null given
  false:           TypeError: zend_array_ht_or_str(): Argument #1 ($param) must be of type array|string, false given
  true:            TypeError: zend_array_ht_or_str(): Argument #1 ($param) must be of type array|string, true given
  42:              TypeError: zend_array_ht_or_str(): Argument #1 ($param) must be of type array|string, int given
  73.5:            TypeError: zend_array_ht_or_str(): Argument #1 ($param) must be of type array|string, float given
  'string':        string(6) "string"
  '15':            string(2) "15"
  '56.7':          string(4) "56.7"
  'stdClass':      string(8) "stdClass"
  anon class name: string(%d) "class@anonymous%s"
  []:              array(0) {}
  new stdClass():  TypeError: zend_array_ht_or_str(): Argument #1 ($param) must be of type array|string, stdClass given
  new S():         TypeError: zend_array_ht_or_str(): Argument #1 ($param) must be of type array|string, S given
  STDOUT:          TypeError: zend_array_ht_or_str(): Argument #1 ($param) must be of type array|string, resource given

Using zend_array_ht_or_str_or_null:

  null:            NULL
  false:           TypeError: zend_array_ht_or_str_or_null(): Argument #1 ($param) must be of type array|string|null, false given
  true:            TypeError: zend_array_ht_or_str_or_null(): Argument #1 ($param) must be of type array|string|null, true given
  42:              TypeError: zend_array_ht_or_str_or_null(): Argument #1 ($param) must be of type array|string|null, int given
  73.5:            TypeError: zend_array_ht_or_str_or_null(): Argument #1 ($param) must be of type array|string|null, float given
  'string':        string(6) "string"
  '15':            string(2) "15"
  '56.7':          string(4) "56.7"
  'stdClass':      string(8) "stdClass"
  anon class name: string(%d) "class@anonymous%s"
  []:              array(0) {}
  new stdClass():  TypeError: zend_array_ht_or_str_or_null(): Argument #1 ($param) must be of type array|string|null, stdClass given
  new S():         TypeError: zend_array_ht_or_str_or_null(): Argument #1 ($param) must be of type array|string|null, S given
  STDOUT:          TypeError: zend_array_ht_or_str_or_null(): Argument #1 ($param) must be of type array|string|null, resource given
