--TEST--
Inline short declaration function dereference
--FILE--
<?php

function a () {
    $a = function() => 123;
    $b = function() => $a;

    return $b;
}

var_dump(get_debug_type( a()() ));

function b () {
    $a = fn() => 123;
    $b = fn() => $a;

    return $b;
}

var_dump(get_debug_type( b()() ));

?>
--EXPECTF--
Warning: Undefined variable $a in %s on line %d
string(4) "null"
string(7) "Closure"