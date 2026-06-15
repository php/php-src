--TEST--
Typed local variables: passing one by reference enforces its type on writes by the callee
--FILE--
<?php
function coerce(&$r) { $r = "5"; }
function bad(&$r)    { $r = "abc"; }

// Weak mode: callee writes a coercible value -> coerced.
int $a = 1;
coerce($a);
var_dump($a);

// Weak mode: callee writes a non-coercible value -> TypeError, value unchanged.
int $b = 2;
try {
    bad($b);
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($b);
?>
--EXPECT--
int(5)
Cannot assign string to reference held by local variable $b of type int
int(2)
