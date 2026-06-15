--TEST--
Typed local variables: int overflow on ++/-- throws TypeError and clamps (matches typed property)
--FILE--
<?php
// Incrementing an int local past PHP_INT_MAX throws and clamps to PHP_INT_MAX.
int $x = PHP_INT_MAX;
try {
    $x++;
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($x === PHP_INT_MAX);

// Decrementing an int local past PHP_INT_MIN throws and clamps to PHP_INT_MIN.
int $y = PHP_INT_MIN;
try {
    --$y;
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($y === PHP_INT_MIN);
?>
--EXPECT--
Cannot increment local variable $x of type int past its maximal value
bool(true)
Cannot decrement local variable $y of type int past its minimal value
bool(true)
