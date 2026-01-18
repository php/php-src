--TEST--
Defer can access exception variable
--FILE--
<?php
function test() {
    $value = "test value";

    defer {
        echo "Defer: $value\n";
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
Defer: test value
Caught: Test
