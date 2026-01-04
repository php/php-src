--TEST--
Exception in defer block replaces original exception
--FILE--
<?php
function test() {
    defer {
        echo "Defer throwing new exception\n";
        throw new Exception("Defer exception");
    }

    throw new Exception("Original exception");
}

try {
    test();
} catch (Exception $e) {
    echo "Caught: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
Defer throwing new exception
Caught: Defer exception
