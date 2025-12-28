--TEST--
Defer executes on exception
--FILE--
<?php
function test() {
    defer {
        echo "Defer executed\n";
    }
    throw new Exception("Test exception");
}

try {
    test();
} catch (Exception $e) {
    echo "Caught: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
Defer executed
Caught: Test exception
