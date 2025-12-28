--TEST--
Defer execution order with multiple returns
--FILE--
<?php
function test($value) {
    echo "Test $value start\n";

    defer {
        echo "Defer 1 for $value\n";
    }

    defer {
        echo "Defer 2 for $value\n";
    }

    if ($value < 0) {
        echo "Returning early for negative\n";
        return "negative";
    }

    defer {
        echo "Defer 3 for $value\n";
    }

    if ($value == 0) {
        echo "Returning early for zero\n";
        return "zero";
    }

    defer {
        echo "Defer 4 for $value\n";
    }

    echo "Normal return for positive\n";
    return "positive";
}

echo "=== Test -1 ===\n";
echo "Result: " . test(-1) . "\n\n";

echo "=== Test 0 ===\n";
echo "Result: " . test(0) . "\n\n";

echo "=== Test 1 ===\n";
echo "Result: " . test(1) . "\n";
?>
--EXPECT--
=== Test -1 ===
Test -1 start
Returning early for negative
Defer 2 for -1
Defer 1 for -1
Result: negative

=== Test 0 ===
Test 0 start
Returning early for zero
Defer 3 for 0
Defer 2 for 0
Defer 1 for 0
Result: zero

=== Test 1 ===
Test 1 start
Normal return for positive
Defer 4 for 1
Defer 3 for 1
Defer 2 for 1
Defer 1 for 1
Result: positive
