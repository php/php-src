--TEST--
Typed local variables: writes through an alias reference ($y = &$x) are type-checked
--FILE--
<?php
// Weak mode: a bad write through the alias throws and leaves the value unchanged.
int $x = 1;
$y = &$x;
try {
    $y = "abc";
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($x);

// A coercible write through the alias succeeds and is coerced.
$y = "5";
var_dump($x);

// Writing through the original variable is enforced too (shared reference).
try {
    $x = [];
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($x);
?>
--EXPECT--
Cannot assign string to reference held by local variable $x of type int
int(1)
int(5)
Cannot assign array to local variable $x of type int
int(5)
