--TEST--
TypeError for compound assignment operations
--FILE--
<?php

$x = [];
try {
    $x += "1";
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $x -= "1";
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $x *= "1";
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $x /= "1";
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $x **= "1";
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $x %= "1";
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $x <<= "1";
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $x >>= "1";
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Unsupported operand types: array + non-numeric string
Unsupported operand types: array - non-numeric string
Unsupported operand types: array * non-numeric string
Unsupported operand types: array / non-numeric string
Unsupported operand types: array ** non-numeric string
Unsupported operand types: array % non-numeric string
Unsupported operand types: array << non-numeric string
Unsupported operand types: array >> non-numeric string
