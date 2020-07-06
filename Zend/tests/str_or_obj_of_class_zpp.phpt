--TEST--
Test Z_PARAM_STR_OR_OBJ_OF_CLASS() and Z_PARAM_STR_OR_OBJ_OF_CLASS_OR_NULL
--SKIPIF--
<?php
if (!extension_loaded('zend-test')) die('skip zend-test extension not loaded');
?>
--FILE--
<?php

class Foo {}

var_dump(zend_string_or_stdclass("string"));
var_dump(zend_string_or_stdclass(1));
var_dump(zend_string_or_stdclass(null));
var_dump(zend_string_or_stdclass(new stdClass()));

try {
    zend_string_or_stdclass([]);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    zend_string_or_stdclass(new Foo());
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump(zend_string_or_stdclass_or_null("string"));
var_dump(zend_string_or_stdclass_or_null(1));
var_dump(zend_string_or_stdclass_or_null(null));
var_dump(zend_string_or_stdclass_or_null(new stdClass()));

try {
    zend_string_or_stdclass_or_null([]);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    zend_string_or_stdclass_or_null(new Foo());
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
string(6) "string"
string(1) "1"
string(0) ""
object(stdClass)#1 (0) {
}
zend_string_or_stdclass(): Argument #1 ($param) must be of type stdClass|string, array given
zend_string_or_stdclass(): Argument #1 ($param) must be of type stdClass|string, Foo given
string(6) "string"
string(1) "1"
NULL
object(stdClass)#1 (0) {
}
zend_string_or_stdclass_or_null(): Argument #1 ($param) must be of type stdClass|string|null, array given
zend_string_or_stdclass_or_null(): Argument #1 ($param) must be of type stdClass|string|null, Foo given
