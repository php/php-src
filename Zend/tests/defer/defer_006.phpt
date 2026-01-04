--TEST--
Defer with loops and early return
--FILE--
<?php
function test($n) {
    echo "Test $n start\n";

    defer {
        echo "Defer for test $n\n";
    }

    for ($i = 0; $i < 5; $i++) {
        echo "Loop $i\n";
        if ($i == 2) {
            return "Early return from $n";
        }
    }

    return "Normal return from $n";
}

$result1 = test(1);
echo "Result: $result1\n";

$result2 = test(2);
echo "Result: $result2\n";
?>
--EXPECT--
Test 1 start
Loop 0
Loop 1
Loop 2
Defer for test 1
Result: Early return from 1
Test 2 start
Loop 0
Loop 1
Loop 2
Defer for test 2
Result: Early return from 2
