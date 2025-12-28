--TEST--
Defer executes even when exception thrown in loop
--FILE--
<?php
function test() {
    defer {
        echo "Defer executed\n";
    }

    for ($i = 0; $i < 5; $i++) {
        echo "Loop $i\n";
        if ($i == 2) {
            throw new Exception("Loop exception");
        }
    }
}

try {
    test();
} catch (Exception $e) {
    echo "Caught: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
Loop 0
Loop 1
Loop 2
Defer executed
Caught: Loop exception
