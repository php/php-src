--TEST--
Test that the mixed return type is not compatible with a void return value
--FILE--
<?php

function foo(): mixed
{
}

try {
    foo();
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
foo(): Return value must be of type mixed, none returned
