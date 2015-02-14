--TEST--
void return type: allowed
--FILE--
<?php

function foo(): void {
    // okay
}

foo();

function bar(): void {
    return; // okay
}

bar();

echo "OK!", PHP_EOL;
--EXPECT--
OK!
