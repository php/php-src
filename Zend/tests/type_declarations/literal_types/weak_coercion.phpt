--TEST--
Literal types: weak-mode coercion (coerce to base, then membership)
--FILE--
<?php
function i(1|2|3 $x): int { return $x; }
function f(1.5|2.5 $x): float { return $x; }

var_dump(i("2"));
var_dump(i(2.0));
var_dump(i(true));
var_dump(f("2.5"));

try {
    i("4");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

function mix(1|2|string $x): string { return $x === 1 ? "int-one" : (string) $x; }
var_dump(mix(1));
var_dump(mix("hello"));
var_dump(mix(5));
?>
--EXPECTF--
int(2)
int(2)
int(1)
float(2.5)
i(): Argument #1 ($x) must be of type 1|2|3, string given, called in %s on line %d
string(7) "int-one"
string(5) "hello"
string(1) "5"
