--TEST--
Test array_ht_or_str ZPP specifier (weak_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

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

  null:            
Deprecated: zend_array_ht_or_str(): Passing null to parameter #1 ($param) of type array|string is deprecated in %s on line %d
string(0) ""
  false:           string(0) ""
  true:            string(1) "1"
  42:              string(2) "42"
  73.5:            string(4) "73.5"
  'string':        string(6) "string"
  '15':            string(2) "15"
  '56.7':          string(4) "56.7"
  'stdClass':      string(8) "stdClass"
  anon class name: string(%d) "class@anonymous%s"
  []:              array(0) {}
  new stdClass():  TypeError: zend_array_ht_or_str(): Argument #1 ($param) must be of type array|string, stdClass given
  new S():         string(7) "S class"
  STDOUT:          TypeError: zend_array_ht_or_str(): Argument #1 ($param) must be of type array|string, resource given

Using zend_array_ht_or_str_or_null:

  null:            NULL
  false:           string(0) ""
  true:            string(1) "1"
  42:              string(2) "42"
  73.5:            string(4) "73.5"
  'string':        string(6) "string"
  '15':            string(2) "15"
  '56.7':          string(4) "56.7"
  'stdClass':      string(8) "stdClass"
  anon class name: string(%d) "class@anonymous%s"
  []:              array(0) {}
  new stdClass():  TypeError: zend_array_ht_or_str_or_null(): Argument #1 ($param) must be of type array|string|null, stdClass given
  new S():         string(7) "S class"
  STDOUT:          TypeError: zend_array_ht_or_str_or_null(): Argument #1 ($param) must be of type array|string|null, resource given
