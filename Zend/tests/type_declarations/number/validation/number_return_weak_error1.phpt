--TEST--
Test that the number return type is not compatible with a void return value
--FILE--
<?php

function foo(): number
{
}

try {
    foo();
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Return value of foo() must be of type number, none returned
