--TEST--
Union of int|string shouldn't warn if string semantics are used
--FILE--
<?php

$float = 1.0;

function foo(int|string $a): void {
    var_dump($a);
}

foo(1.0);
foo(1.5);
foo(fdiv(0, 0));
foo(10e120);
foo(10e500); // Infinity



?>
--EXPECTF--
int(1)

Deprecated: Implicit conversion from float 1.5 to int loses precision in %s on line %d
int(1)
string(3) "NAN"
string(8) "1.0E+121"
string(3) "INF"
