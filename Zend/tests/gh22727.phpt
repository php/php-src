--TEST--
GH-22727: Return opcodes use the return statement line for multiline expressions
--FILE--
<?php

function test(): int
{
    return match (true) {
        true => 'not an int',
    };
}

try {
    test();
} catch (Throwable $e) {
    echo $e::class, ': on line ', $e->getLine(), "\n";
}

?>
--EXPECT--
TypeError: on line 5
