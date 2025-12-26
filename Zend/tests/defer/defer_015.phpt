--TEST--
Defer basic exception that propagates
--FILE--
<?php
function test() {
    echo "Start\n";

    defer {
        echo "Defer\n";
    }

    throw new Exception("Test");
}

try {
    test();
} catch (Exception $e) {
    echo "Caught\n";
}
?>
--EXPECT--
Start
Defer
Caught
