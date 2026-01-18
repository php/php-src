--TEST--
Multiple defers execute on exception in LIFO order
--FILE--
<?php
function test() {
    defer {
        echo "Defer 1\n";
    }
    defer {
        echo "Defer 2\n";
    }
    defer {
        echo "Defer 3\n";
    }
    throw new Exception("Test");
}

try {
    test();
} catch (Exception $e) {
    echo "Caught: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
Defer 3
Defer 2
Defer 1
Caught: Test
