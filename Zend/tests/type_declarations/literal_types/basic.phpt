--TEST--
Literal types: basic accept and reject (int, float, string)
--FILE--
<?php
function i(1|2|3 $x): int { return $x; }
function f(1.5|2.5 $x): float { return $x; }
function s('a'|'b' $x): string { return $x; }

var_dump(i(1), i(2), i(3));
var_dump(f(1.5), f(2.5));
var_dump(s('a'), s('b'));

try {
    i(4);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    f(3.5);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    s('c');
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
int(1)
int(2)
int(3)
float(1.5)
float(2.5)
string(1) "a"
string(1) "b"
i(): Argument #1 ($x) must be of type 1|2|3, int given, called in %s on line %d
f(): Argument #1 ($x) must be of type 1.5|2.5, float given, called in %s on line %d
s(): Argument #1 ($x) must be of type 'a'|'b', string given, called in %s on line %d
