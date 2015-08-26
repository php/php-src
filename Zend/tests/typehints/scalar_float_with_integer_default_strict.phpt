--TEST--
Float type hint should allow an integer as default even with strict types
--FILE--
<?php

declare(strict_types=1);

function test(float $arg = 0)
{
    var_dump($arg);
}

test();

?>
--EXPECT--
float(0)
