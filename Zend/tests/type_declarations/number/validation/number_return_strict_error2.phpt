--TEST--
Test that the number return type is not compatible with a void return value in strict mode
--FILE--
<?php
declare(strict_types=1);

function foo($value): number
{
    return $value;
}

try {
    foo(null);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    foo(false);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    foo(true);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    foo("0");
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
Return value of foo() must be of type number, bool returned
Return value of foo() must be of type number, bool returned
Return value of foo() must be of type number, string returned
Return value of foo() must be of type number, array returned
Return value of foo() must be of type number, object returned
Return value of foo() must be of type number, resource returned
