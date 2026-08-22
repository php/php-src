--TEST--
Test str ZPP specifier (weak_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

$types = require 'types.inc';
$functions = [
    'zend_str',
    'zend_str_or_null',
    'zend_str_slow_zpp',
    'zend_str_or_null_slow_zpp',
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
Using zend_str:

  null:            
Deprecated: zend_str(): Passing null to parameter #1 ($param) of type string is deprecated in %s on line %d
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
  []:              TypeError: zend_str(): Argument #1 ($param) must be of type string, array given
  new stdClass():  TypeError: zend_str(): Argument #1 ($param) must be of type string, stdClass given
  new S():         string(7) "S class"
  STDOUT:          TypeError: zend_str(): Argument #1 ($param) must be of type string, resource given

Using zend_str_or_null:

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
  []:              TypeError: zend_str_or_null(): Argument #1 ($param) must be of type ?string, array given
  new stdClass():  TypeError: zend_str_or_null(): Argument #1 ($param) must be of type ?string, stdClass given
  new S():         string(7) "S class"
  STDOUT:          TypeError: zend_str_or_null(): Argument #1 ($param) must be of type ?string, resource given

Using zend_str_slow_zpp:

  null:            
Deprecated: zend_str_slow_zpp(): Passing null to parameter #1 ($param) of type string is deprecated in %s on line %d
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
  []:              TypeError: zend_str_slow_zpp(): Argument #1 ($param) must be of type string, array given
  new stdClass():  TypeError: zend_str_slow_zpp(): Argument #1 ($param) must be of type string, stdClass given
  new S():         string(7) "S class"
  STDOUT:          TypeError: zend_str_slow_zpp(): Argument #1 ($param) must be of type string, resource given

Using zend_str_or_null_slow_zpp:

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
  []:              TypeError: zend_str_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, array given
  new stdClass():  TypeError: zend_str_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, stdClass given
  new S():         string(7) "S class"
  STDOUT:          TypeError: zend_str_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, resource given
