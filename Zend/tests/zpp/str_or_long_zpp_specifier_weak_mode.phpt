--TEST--
Test str_or_long ZPP specifier (weak_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

$types = require 'types.inc';
$functions = [
    'zend_str_or_long',
    'zend_str_or_long_or_null',
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
Using zend_str_or_long:

  null:            
Deprecated: zend_str_or_long(): Passing null to parameter #1 ($param) of type string|int is deprecated in %s on line %d
int(0)
  false:           int(0)
  true:            int(1)
  42:              int(42)
  73.5:            
Deprecated: Implicit conversion from float 73.5 to int loses precision in %s on line %d
int(73)
  'string':        string(6) "string"
  '15':            string(2) "15"
  '56.7':          string(4) "56.7"
  'stdClass':      string(8) "stdClass"
  anon class name: string(%d) "class@anonymous%s"
  []:              TypeError: zend_str_or_long(): Argument #1 ($param) must be of type string|int, array given
  new stdClass():  TypeError: zend_str_or_long(): Argument #1 ($param) must be of type string|int, stdClass given
  new S():         string(7) "S class"
  STDOUT:          TypeError: zend_str_or_long(): Argument #1 ($param) must be of type string|int, resource given

Using zend_str_or_long_or_null:

  null:            NULL
  false:           int(0)
  true:            int(1)
  42:              int(42)
  73.5:            
Deprecated: Implicit conversion from float 73.5 to int loses precision in %s on line %d
int(73)
  'string':        string(6) "string"
  '15':            string(2) "15"
  '56.7':          string(4) "56.7"
  'stdClass':      string(8) "stdClass"
  anon class name: string(%d) "class@anonymous%s"
  []:              TypeError: zend_str_or_long_or_null(): Argument #1 ($param) must be of type string|int|null, array given
  new stdClass():  TypeError: zend_str_or_long_or_null(): Argument #1 ($param) must be of type string|int|null, stdClass given
  new S():         string(7) "S class"
  STDOUT:          TypeError: zend_str_or_long_or_null(): Argument #1 ($param) must be of type string|int|null, resource given
