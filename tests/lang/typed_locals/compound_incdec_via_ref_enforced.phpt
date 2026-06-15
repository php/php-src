--TEST--
Typed local variables: compound assignment and ++/-- through an alias reference stay enforced
--FILE--
<?php
// A typed local aliased by a reference still enforces its type on += through
// either name (the type travels with the reference's type-source).
int $x = 1;
$r = &$x;
$x += 0.5;          // weak: deprecation + truncation
var_dump($x);       // int(1)

int $y = 1;
$s = &$y;
$s += 0.5;          // writing via the other name is enforced too
var_dump($y);       // int(1)

// int overflow on ++ through an aliased typed local throws and clamps.
int $z = PHP_INT_MAX;
$t = &$z;
try {
    $z++;
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($z === PHP_INT_MAX);
?>
--EXPECTF--
Deprecated: Implicit conversion from float 1.5 to int loses precision in %s on line %d
int(1)

Deprecated: Implicit conversion from float 1.5 to int loses precision in %s on line %d
int(1)
Cannot increment a reference held by local variable $z of type int past its maximal value
bool(true)
