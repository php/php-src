--TEST--
Test that the number parameter type accepts any kind of numeric argument in weak mode
--FILE--
<?php

function foo(number $a)
{
    var_dump($a);
}

foo(false);
foo(true);
foo(1);
foo(3.14);
foo("0");
foo("12foo");

?>
--EXPECTF--
int(0)
int(1)
int(1)
float(3.14)
int(0)

Notice: A non well formed numeric value encountered in %s on line %d
int(12)
