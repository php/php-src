--TEST--
Test Z_PARAM_OBJ_OF_CLASS_OR_STR() and Z_PARAM_OBJ_OF_CLASS_OR_STR_OR_NULL
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

var_dump(zend_string_or_stdclass("string"));
var_dump(zend_string_or_stdclass(1));
var_dump(zend_string_or_stdclass(null));
var_dump(zend_string_or_stdclass(new stdClass()));
var_dump(zend_string_or_stdclass(new ToString()));

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
var_dump(zend_string_or_stdclass_or_null(new ToString()));

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
--EXPECTF--
string(6) "string"
string(1) "1"

Deprecated: zend_string_or_stdclass(): Passing null to parameter #1 ($param) of type string is deprecated in %s on line %d
string(0) ""
object(stdClass)#1 (0) {
}
string(8) "ToString"
zend_string_or_stdclass(): Argument #1 ($param) must be of type stdClass|string, array given
zend_string_or_stdclass(): Argument #1 ($param) must be of type stdClass|string, Foo given
string(6) "string"
string(1) "1"
NULL
object(stdClass)#1 (0) {
}
string(8) "ToString"
zend_string_or_stdclass_or_null(): Argument #1 ($param) must be of type stdClass|string|null, array given
zend_string_or_stdclass_or_null(): Argument #1 ($param) must be of type stdClass|string|null, Foo given
