--TEST--
Bug #37811 (define not using toString on objects)
--FILE--
<?php

class TestClass
{
    function __toString()
    {
        return "Foo";
    }
}

define("Bar", new TestClass);
var_dump(Bar);

try {
    define("Baz", new stdClass);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    var_dump(Baz);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
string(3) "Foo"
define(): Argument #2 ($value) cannot be an object, stdClass given
Undefined constant "Baz"
