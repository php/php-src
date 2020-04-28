--TEST--
Test that the number return type is not compatible with non-numeric return values
--FILE--
<?php

function foo($a): number
{
    return $a;
}

try {
    foo(null);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    foo([]);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    foo(new stdClass());
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    foo(fopen(__FILE__, "r"));
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Return value of foo() must be of type number, null returned
Return value of foo() must be of type number, array returned
Return value of foo() must be of type number, object returned
Return value of foo() must be of type number, resource returned
