--TEST--
Test string_param ZPP specifier (strict_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

declare(strict_types=1);

$types = require 'types.inc';
$functions = [
    'zend_string_param',
    'zend_string_param_or_null',
    'zend_string_param_slow_zpp',
    'zend_string_param_or_null_slow_zpp',
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
Using zend_string_param:

  null:            TypeError: zend_string_param(): Argument #1 ($param) must be of type string, null given
  false:           TypeError: zend_string_param(): Argument #1 ($param) must be of type string, false given
  true:            TypeError: zend_string_param(): Argument #1 ($param) must be of type string, true given
  42:              TypeError: zend_string_param(): Argument #1 ($param) must be of type string, int given
  73.5:            TypeError: zend_string_param(): Argument #1 ($param) must be of type string, float given
  'string':        string(6) "string"
  '15':            string(2) "15"
  '56.7':          string(4) "56.7"
  'stdClass':      string(8) "stdClass"
  anon class name: string(%d) "class@anonymous%s"
  []:              TypeError: zend_string_param(): Argument #1 ($param) must be of type string, array given
  new stdClass():  TypeError: zend_string_param(): Argument #1 ($param) must be of type string, stdClass given
  new S():         TypeError: zend_string_param(): Argument #1 ($param) must be of type string, S given
  STDOUT:          TypeError: zend_string_param(): Argument #1 ($param) must be of type string, resource given

Using zend_string_param_or_null:

  null:            NULL
  false:           TypeError: zend_string_param_or_null(): Argument #1 ($param) must be of type ?string, false given
  true:            TypeError: zend_string_param_or_null(): Argument #1 ($param) must be of type ?string, true given
  42:              TypeError: zend_string_param_or_null(): Argument #1 ($param) must be of type ?string, int given
  73.5:            TypeError: zend_string_param_or_null(): Argument #1 ($param) must be of type ?string, float given
  'string':        string(6) "string"
  '15':            string(2) "15"
  '56.7':          string(4) "56.7"
  'stdClass':      string(8) "stdClass"
  anon class name: string(%d) "class@anonymous%s"
  []:              TypeError: zend_string_param_or_null(): Argument #1 ($param) must be of type ?string, array given
  new stdClass():  TypeError: zend_string_param_or_null(): Argument #1 ($param) must be of type ?string, stdClass given
  new S():         TypeError: zend_string_param_or_null(): Argument #1 ($param) must be of type ?string, S given
  STDOUT:          TypeError: zend_string_param_or_null(): Argument #1 ($param) must be of type ?string, resource given

Using zend_string_param_slow_zpp:

  null:            TypeError: zend_string_param_slow_zpp(): Argument #1 ($param) must be of type string, null given
  false:           TypeError: zend_string_param_slow_zpp(): Argument #1 ($param) must be of type string, false given
  true:            TypeError: zend_string_param_slow_zpp(): Argument #1 ($param) must be of type string, true given
  42:              TypeError: zend_string_param_slow_zpp(): Argument #1 ($param) must be of type string, int given
  73.5:            TypeError: zend_string_param_slow_zpp(): Argument #1 ($param) must be of type string, float given
  'string':        string(6) "string"
  '15':            string(2) "15"
  '56.7':          string(4) "56.7"
  'stdClass':      string(8) "stdClass"
  anon class name: string(%d) "class@anonymous%s"
  []:              TypeError: zend_string_param_slow_zpp(): Argument #1 ($param) must be of type string, array given
  new stdClass():  TypeError: zend_string_param_slow_zpp(): Argument #1 ($param) must be of type string, stdClass given
  new S():         TypeError: zend_string_param_slow_zpp(): Argument #1 ($param) must be of type string, S given
  STDOUT:          TypeError: zend_string_param_slow_zpp(): Argument #1 ($param) must be of type string, resource given

Using zend_string_param_or_null_slow_zpp:

  null:            NULL
  false:           TypeError: zend_string_param_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, false given
  true:            TypeError: zend_string_param_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, true given
  42:              TypeError: zend_string_param_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, int given
  73.5:            TypeError: zend_string_param_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, float given
  'string':        string(6) "string"
  '15':            string(2) "15"
  '56.7':          string(4) "56.7"
  'stdClass':      string(8) "stdClass"
  anon class name: string(%d) "class@anonymous%s"
  []:              TypeError: zend_string_param_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, array given
  new stdClass():  TypeError: zend_string_param_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, stdClass given
  new S():         TypeError: zend_string_param_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, S given
  STDOUT:          TypeError: zend_string_param_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, resource given
