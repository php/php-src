--TEST--
Defining constants with non-scalar values
--FILE--
<?php

try {
    define('foo', new stdClass);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    var_dump(foo);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

define('foo', fopen(__FILE__, 'r'));
var_dump(foo);

?>
--EXPECT--
define(): Argument #2 ($value) cannot be an object, stdClass given
Undefined constant "foo"
resource(5) of type (stream)
