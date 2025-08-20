--TEST--
Test Z_PARAM_OBJ_OR_STR() and Z_PARAM_OBJ_OR_STR_OR_NULL
--EXTENSIONS--
zend_test
--FILE--
<?php

class Foo {}

var_dump(zend_string_or_object("string"));
var_dump(zend_string_or_object(1));
var_dump(zend_string_or_object(null));
var_dump(zend_string_or_object(new stdClass()));
var_dump(zend_string_or_object(new Foo()));

try {
    zend_string_or_object([]);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump(zend_string_or_object_or_null("string"));
var_dump(zend_string_or_object_or_null(1));
var_dump(zend_string_or_object_or_null(null));
var_dump(zend_string_or_object_or_null(new stdClass()));
var_dump(zend_string_or_object_or_null(new Foo()));

try {
    zend_string_or_object_or_null([]);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECTF--
string(6) "string"
string(1) "1"

Deprecated: zend_string_or_object(): Passing null to parameter #1 ($param) of type object|string is deprecated in %s on line %d
string(0) ""
object(stdClass)#1 (0) {
}
object(Foo)#1 (0) {
}
zend_string_or_object(): Argument #1 ($param) must be of type object|string, array given
string(6) "string"
string(1) "1"
NULL
object(stdClass)#2 (0) {
}
object(Foo)#2 (0) {
}
zend_string_or_object_or_null(): Argument #1 ($param) must be of type object|string|null, array given
