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
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: foo(): Return value must be of type mixed, none returned
