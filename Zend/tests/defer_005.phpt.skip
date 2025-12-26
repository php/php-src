--TEST--
Defer executes on exception
--FILE--
<?php
function test_defer_on_exception() {
    echo "Start\n";

    defer {
        echo "Cleanup executed\n";
    }

    echo "Before throw\n";
    throw new Exception("Test");
}

try {
    test_defer_on_exception();
} catch (Exception $e) {
    echo "Caught: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
Start
Before throw
Cleanup executed
Caught: Test
