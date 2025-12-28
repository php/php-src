--TEST--
Defer with conditional exception
--FILE--
<?php
function test($throw) {
    defer {
        echo "Defer executed\n";
    }

    if ($throw) {
        throw new Exception("Conditional exception");
    }

    echo "Normal execution\n";
}

try {
    test(false);
    echo "---\n";
    test(true);
} catch (Exception $e) {
    echo "Caught: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
Normal execution
Defer executed
---
Defer executed
Caught: Conditional exception
