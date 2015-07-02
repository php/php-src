--TEST--
Float type hint should allow an integer as default
--FILE--
<?php

function test(float $arg = 0)
{
    var_dump($arg);
}

test();

?>
--EXPECT--
float(0)
