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
    echo $e->getMessage(), "\n";
}

try {
    Test::{$null?->foo}()->bar;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Access to undeclared static property Test::$
Method name must be a string
