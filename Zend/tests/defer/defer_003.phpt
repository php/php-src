--TEST--
Defer executes on return
--FILE--
<?php
function test_defer_on_return() {
    echo "Start\n";

    defer {
        echo "Cleanup\n";
    }

    echo "Before return\n";
    return "Result";
}

$result = test_defer_on_return();
echo "Got: $result\n";
?>
--EXPECT--
Start
Before return
Cleanup
Got: Result
