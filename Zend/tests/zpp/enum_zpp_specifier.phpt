--TEST--
Test Z_PARAM_ENUM specifier
--EXTENSIONS--
zend_test
--FILE--
<?php

$types = require 'types.inc';
$types['ZendTestUnitEnum::Foo'] = ZendTestUnitEnum::Foo;

$functions = [
    'zend_enum',
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
Using zend_enum:

  null:            TypeError: zend_enum(): Argument #1 ($param) must be of type ZendTestUnitEnum, null given
  false:           TypeError: zend_enum(): Argument #1 ($param) must be of type ZendTestUnitEnum, false given
  true:            TypeError: zend_enum(): Argument #1 ($param) must be of type ZendTestUnitEnum, true given
  42:              TypeError: zend_enum(): Argument #1 ($param) must be of type ZendTestUnitEnum, int given
  73.5:            TypeError: zend_enum(): Argument #1 ($param) must be of type ZendTestUnitEnum, float given
  'string':        TypeError: zend_enum(): Argument #1 ($param) must be of type ZendTestUnitEnum, string given
  '15':            TypeError: zend_enum(): Argument #1 ($param) must be of type ZendTestUnitEnum, string given
  '56.7':          TypeError: zend_enum(): Argument #1 ($param) must be of type ZendTestUnitEnum, string given
  'stdClass':      TypeError: zend_enum(): Argument #1 ($param) must be of type ZendTestUnitEnum, string given
  anon class name: TypeError: zend_enum(): Argument #1 ($param) must be of type ZendTestUnitEnum, string given
  []:              TypeError: zend_enum(): Argument #1 ($param) must be of type ZendTestUnitEnum, array given
  new stdClass():  TypeError: zend_enum(): Argument #1 ($param) must be of type ZendTestUnitEnum, stdClass given
  new S():         TypeError: zend_enum(): Argument #1 ($param) must be of type ZendTestUnitEnum, S given
  STDOUT:          TypeError: zend_enum(): Argument #1 ($param) must be of type ZendTestUnitEnum, resource given
  ZendTestUnitEnum::Foo: int(1)
