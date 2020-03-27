--TEST--
Test that the mixed return type is not compatible with a void return value in strict mode
--FILE--
<?php
declare(strict_types=1);

function foo(): mixed
{
}

try {
    foo();
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECTF--
Return value of foo() must be of type mixed, none returned

