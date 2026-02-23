--TEST--
GH-20262: Reentrancy handling in transitive comparison mode
--FILE--
<?php

echo "Test 1: usort() calling sort() internally (reentrancy)\n";
$data = ['c', 'a', 'b'];
$inner_calls = 0;

usort($data, function($a, $b) use (&$inner_calls) {
    // Call sort() during comparison to test reentrancy
    if ($inner_calls < 3) {
        $inner_calls++;
        $temp = ['z', 'x', 'y'];
        sort($temp, SORT_REGULAR);
        // Verify inner sort worked correctly
        if ($temp !== ['x', 'y', 'z']) {
            echo "FAIL: Inner sort incorrect\n";
        }
    }
    return $a <=> $b;
});

echo "Result: " . implode(', ', $data) . "\n";
echo "Inner sort calls: $inner_calls\n";

echo "\nTest 2: Nested sort() with SORT_REGULAR\n";
function outer_sort() {
    $arr = ['5', '10', '3A'];
    sort($arr, SORT_REGULAR);
    return $arr;
}

function nested_sort() {
    $outer = outer_sort();
    $inner = outer_sort();

    // Both should produce identical results
    if ($outer === $inner) {
        echo "Nested sorts consistent\n";
    } else {
        echo "FAIL: Nested sorts inconsistent\n";
    }

    return $outer;
}

$result = nested_sort();
echo "Result: " . implode(', ', $result) . "\n";

echo "\nTest 3: array_unique() during comparison\n";
$test_data = ['a', 'b', 'c'];
$call_count = 0;

usort($test_data, function($a, $b) use (&$call_count) {
    $call_count++;
    // Call array_unique with SORT_REGULAR during comparison
    if ($call_count === 1) {
        $temp = ['x', 'x', 'y'];
        $unique = array_unique($temp, SORT_REGULAR);
        if (count($unique) === 2) {
            echo "Inner array_unique worked correctly\n";
        }
    }
    return strcmp($a, $b);
});

echo "Outer usort completed\n";

echo "\nTest 4: uksort() calling ksort() (key comparison reentrancy)\n";
$arr = ['5' => 'a', '10' => 'b', '3A' => 'c'];
$inner_calls = 0;

uksort($arr, function($a, $b) use (&$inner_calls) {
    // Call ksort during key comparison to test reentrancy
    if ($inner_calls < 2) {
        $inner_calls++;
        $inner = ['x' => 1, 'y' => 2, 'z' => 3];
        ksort($inner, SORT_REGULAR);
        if (array_keys($inner) === ['x', 'y', 'z']) {
            echo "Inner ksort in uksort worked correctly\n";
        }
    }
    return strcmp($a, $b);
});

echo "Outer uksort completed: " . implode(', ', array_keys($arr)) . "\n";

?>
--EXPECT--
Test 1: usort() calling sort() internally (reentrancy)
Result: a, b, c
Inner sort calls: 3

Test 2: Nested sort() with SORT_REGULAR
Nested sorts consistent
Result: 5, 10, 3A

Test 3: array_unique() during comparison
Inner array_unique worked correctly
Outer usort completed

Test 4: uksort() calling ksort() (key comparison reentrancy)
Inner ksort in uksort worked correctly
Inner ksort in uksort worked correctly
Outer uksort completed: 10, 3A, 5
