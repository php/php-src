--TEST--
Test Z_PARAM_NUMBER_OR_STR() and Z_PARAM_NUMBER_OR_STR_OR_NULL
--EXTENSIONS--
zend_test
--FILE--
<?php

class Foo {}
class ToString {
    public function __toString() {
        return "ToString";
    }
}

var_dump(zend_number_or_string("string"));
var_dump(zend_number_or_string(1));
var_dump(zend_number_or_string(5.5));
var_dump(zend_number_or_string(null));
var_dump(zend_number_or_string(false));
var_dump(zend_number_or_string(true));
var_dump(zend_number_or_string(new ToString()));

try {
    zend_string_or_object([]);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}
try {
    zend_number_or_string(new Foo());
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump(zend_number_or_string_or_null("string"));
var_dump(zend_number_or_string_or_null(1));
var_dump(zend_number_or_string_or_null(5.5));
var_dump(zend_number_or_string_or_null(null));
var_dump(zend_number_or_string_or_null(false));
var_dump(zend_number_or_string_or_null(true));
var_dump(zend_number_or_string_or_null(new ToString()));

try {
    zend_number_or_string_or_null([]);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}
try {
    zend_number_or_string_or_null(new Foo());
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECTF--
string(6) "string"
int(1)
float(5.5)

Deprecated: zend_number_or_string(): Passing null to parameter #1 ($param) of type string|int|float is deprecated in %s on line %d
int(0)
int(0)
int(1)
string(8) "ToString"
zend_string_or_object(): Argument #1 ($param) must be of type object|string, array given
zend_number_or_string(): Argument #1 ($param) must be of type string|int|float, Foo given
string(6) "string"
int(1)
float(5.5)
NULL
int(0)
int(1)
string(8) "ToString"
zend_number_or_string_or_null(): Argument #1 ($param) must be of type string|int|float|null, array given
zend_number_or_string_or_null(): Argument #1 ($param) must be of type string|int|float|null, Foo given
