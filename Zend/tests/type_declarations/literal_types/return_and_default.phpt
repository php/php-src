--TEST--
Literal types: return types, default values, single and nullable literal types
--FILE--
<?php
function r(int $x): 1|2|3 { return $x; }
var_dump(r(2));
try {
    r(5);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

function d(1|2 $x = 2): int { return $x; }
function df(1.0|2.5 $x = 1): float { return $x; }
var_dump(d(), df());

function one(42 $x): 42 { return $x; }
var_dump(one(42));
try {
    one(43);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

function n(1|2|null $x): null|int { return $x; }
var_dump(n(1), n(null));
?>
--EXPECTF--
int(2)
r(): Return value must be of type 1|2|3, int returned
int(2)
float(1)
int(42)
one(): Argument #1 ($x) must be of type 42, int given, called in %s on line %d
int(1)
NULL
