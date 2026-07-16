--TEST--
Literal types: negative int and float literals
--FILE--
<?php
function i(-1|-2 $x): int { return $x; }
function f(-1.5|2.5 $x): float { return $x; }

var_dump(i(-1), i(-2));
var_dump(f(-1.5), f(2.5));

try {
    i(1);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
int(-1)
int(-2)
float(-1.5)
float(2.5)
i(): Argument #1 ($x) must be of type -1|-2, int given, called in %s on line %d
