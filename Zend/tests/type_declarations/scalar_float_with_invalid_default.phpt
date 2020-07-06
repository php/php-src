--TEST--
Float type should not allow invalid types as default
--FILE--
<?php

function test(float $arg = true)
{
    var_dump($arg);
}

test();

?>
--EXPECTF--
Fatal error: test(): Parameter #1 ($arg) of type float cannot have a default value of type bool in %s on line %d
