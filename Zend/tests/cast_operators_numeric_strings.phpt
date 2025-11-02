--TEST--
Cast operators with numeric strings
--FILE--
<?php

var_dump((?int) "123");
var_dump((!int) "123");
var_dump((?float) "45.67");
var_dump((!float) "45.67");

var_dump((?int) "  89  ");
var_dump((!int) "  89  ");

try {
    var_dump((?int) "123abc");
} catch (TypeError $e) {
    echo "Caught: " . $e->getMessage() . "\n";
}

try {
    var_dump((!float) "12.5xyz");
} catch (TypeError $e) {
    echo "Caught: " . $e->getMessage() . "\n";
}

// Completely non-numeric - should fail
try {
    var_dump((?int) "hello");
} catch (TypeError $e) {
    echo "Caught: " . $e->getMessage() . "\n";
}

?>
--EXPECT--
int(123)
int(123)
float(45.67)
float(45.67)
int(89)
int(89)
Caught: Cannot cast string to int
Caught: Cannot cast string to float
Caught: Cannot cast string to int
