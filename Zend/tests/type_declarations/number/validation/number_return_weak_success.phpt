--TEST--
Test that the number return type is compatible with any numeric value in weak mode
--FILE--
<?php

function foo($a): number
{
    return $a;
}

var_dump(foo(false));
var_dump(foo(true));
var_dump(foo(1));
var_dump(foo(3.14));
var_dump(foo("0foo"));

?>
--EXPECTF--
int(0)
int(1)
int(1)
float(3.14)

Notice: A non well formed numeric value encountered in %s on line %d
int(0)
