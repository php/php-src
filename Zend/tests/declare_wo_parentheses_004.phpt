--TEST--
Testing multiple declare w/o parentheses in one statement
--FILE--
<?php

declare ticks = 1, strict_types = 1;

function tick_handler()
{
    echo "tick_handler() called\n";
}

register_tick_function('tick_handler');

echo (fn(string $fizz) => $fizz . "Buzz\n")('Fizz');

?>
--EXPECT--
tick_handler() called
FizzBuzz
tick_handler() called