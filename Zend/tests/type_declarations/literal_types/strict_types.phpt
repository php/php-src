--TEST--
Literal types: strict_types disallows coercion (except int -> float literal)
--FILE--
<?php
declare(strict_types=1);

function i(1|2|3 $x): int { return $x; }
function f(1.5|2.0 $x): float { return $x; }
function s('a'|'b' $x): string { return $x; }

var_dump(i(2));
var_dump(f(1.5));
var_dump(f(2));
var_dump(s('a'));

foreach ([['i', "2"], ['i', 2.0], ['f', 3], ['s', 1]] as [$fn, $arg]) {
    try {
        $fn($arg);
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
}
?>
--EXPECTF--
int(2)
float(1.5)
float(2)
string(1) "a"
i(): Argument #1 ($x) must be of type 1|2|3, string given, called in %s on line %d
i(): Argument #1 ($x) must be of type 1|2|3, float given, called in %s on line %d
f(): Argument #1 ($x) must be of type 1.5|2.0, int given, called in %s on line %d
s(): Argument #1 ($x) must be of type 'a'|'b', int given, called in %s on line %d
