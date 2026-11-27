--TEST--
GH-25061 ($x = $x - 1 on null is optimized to PRE_DEC)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php

function nullable(?int $x)
{
    $x = $x - 1;
    return $x;
}

function undef()
{
    $x = $x - 1;
    return $x;
}

function twice(?int $x)
{
    $x = $x - 1;
    $x = $x - 1;
    return $x;
}

var_dump(nullable(null));
var_dump(nullable(5));
var_dump(undef());
var_dump(twice(null));

?>
--EXPECTF--
int(-1)
int(4)

Warning: Undefined variable $x in %s on line %d
int(-1)
int(-2)
