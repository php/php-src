--TEST--
Test str_or_long ZPP specifier (strict_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

declare(strict_types=1);

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

  null:            TypeError: zend_str_or_long(): Argument #1 ($param) must be of type string|int, null given
  false:           TypeError: zend_str_or_long(): Argument #1 ($param) must be of type string|int, false given
  true:            TypeError: zend_str_or_long(): Argument #1 ($param) must be of type string|int, true given
  42:              int(42)
  73.5:            TypeError: zend_str_or_long(): Argument #1 ($param) must be of type string|int, float given
  'string':        string(6) "string"
  '15':            string(2) "15"
  '56.7':          string(4) "56.7"
  'stdClass':      string(8) "stdClass"
  anon class name: string(%d) "class@anonymous%s"
  []:              TypeError: zend_str_or_long(): Argument #1 ($param) must be of type string|int, array given
  new stdClass():  TypeError: zend_str_or_long(): Argument #1 ($param) must be of type string|int, stdClass given
  new S():         TypeError: zend_str_or_long(): Argument #1 ($param) must be of type string|int, S given
  STDOUT:          TypeError: zend_str_or_long(): Argument #1 ($param) must be of type string|int, resource given

Using zend_str_or_long_or_null:

  null:            NULL
  false:           TypeError: zend_str_or_long_or_null(): Argument #1 ($param) must be of type string|int|null, false given
  true:            TypeError: zend_str_or_long_or_null(): Argument #1 ($param) must be of type string|int|null, true given
  42:              int(42)
  73.5:            TypeError: zend_str_or_long_or_null(): Argument #1 ($param) must be of type string|int|null, float given
  'string':        string(6) "string"
  '15':            string(2) "15"
  '56.7':          string(4) "56.7"
  'stdClass':      string(8) "stdClass"
  anon class name: string(%d) "class@anonymous%s"
  []:              TypeError: zend_str_or_long_or_null(): Argument #1 ($param) must be of type string|int|null, array given
  new stdClass():  TypeError: zend_str_or_long_or_null(): Argument #1 ($param) must be of type string|int|null, stdClass given
  new S():         TypeError: zend_str_or_long_or_null(): Argument #1 ($param) must be of type string|int|null, S given
  STDOUT:          TypeError: zend_str_or_long_or_null(): Argument #1 ($param) must be of type string|int|null, resource given
