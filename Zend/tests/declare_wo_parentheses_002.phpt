--TEST--
Testing declare w/o parentheses statement strict_types
--FILE--
<?php

declare ticks = 1;
declare strict_types = 1;

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