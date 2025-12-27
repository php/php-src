--TEST--
Defer with nested function calls and exceptions
--FILE--
<?php
function inner() {
    defer {
        echo "Inner defer\n";
    }
    throw new Exception("Inner exception");
}

function outer() {
    defer {
        echo "Outer defer\n";
    }
    inner();
}

try {
    outer();
} catch (Exception $e) {
    echo "Caught: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
Inner defer
Outer defer
Caught: Inner exception
