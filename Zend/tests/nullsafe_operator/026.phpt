--TEST--
Nullsafe chain in static property / method name
--FILE--
<?php

class Test {
}

$null = null;

try {
    Test::${$null?->foo}->bar;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    Test::{$null?->foo}()->bar;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Access to undeclared static property Test::$
Error: Method name must be a string
