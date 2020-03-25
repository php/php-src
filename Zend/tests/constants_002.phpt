--TEST--
Defining constants with non-scalar values
--FILE--
<?php

define('foo', new stdClass);
try {
    var_dump(foo);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

define('foo', fopen(__FILE__, 'r'));
var_dump(foo);

?>
--EXPECTF--
Warning: Constants may only evaluate to scalar values, arrays or resources in %s on line %d
Undefined constant 'foo'
resource(5) of type (stream)
