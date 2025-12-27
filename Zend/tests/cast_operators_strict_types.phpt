--TEST--
Nullable and non-null casts ignore strict_types declaration
--FILE--
<?php
declare(strict_types=1);

var_dump((?int) "123");
var_dump((!int) "456");

try {
    var_dump((?int) "123aze");
} catch (TypeError $e) {
    echo "Caught: " . $e->getMessage() . "\n";
}

try {
    var_dump((!int) "abc");
} catch (TypeError $e) {
    echo "Caught: " . $e->getMessage() . "\n";
}

// Null handling
var_dump((?int) null);
try {
    var_dump((!int) null);
} catch (TypeError $e) {
    echo "Caught: " . $e->getMessage() . "\n";
}

?>
--EXPECT--
int(123)
int(456)
Caught: Cannot cast string to int
Caught: Cannot cast string to int
NULL
Caught: Cannot cast null to int
