--TEST--
Typed local variables: aliasing a typed local to a source ($typed = &$src) checks the source value
--FILE--
<?php
// The value being aliased must satisfy the typed target up front.
$s = "abc";
int $y = 0;
try {
    $y = &$s;
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($y, $s);

// A compatible source aliases fine, and the type is then enforced on the shared reference.
$n = 7;
int $z = 0;
$z = &$n;
$n = 9;
var_dump($z);
try {
    $n = "nope";
} catch (\TypeError $e) {
    echo "TypeError\n";
}
var_dump($z);
?>
--EXPECT--
Cannot assign string to local variable $y of type int
int(0)
string(3) "abc"
int(9)
TypeError
int(9)
