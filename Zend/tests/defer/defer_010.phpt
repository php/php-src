--TEST--
Defer with array operations and modifications
--FILE--
<?php
function test() {
    $data = ['items' => []];

    defer {
        echo "Final data: " . json_encode($data) . "\n";
    }

    defer {
        $data['items'][] = 'cleanup';
        echo "Added cleanup item\n";
    }

    $data['items'][] = 'first';
    $data['items'][] = 'second';
    echo "Current data: " . json_encode($data) . "\n";

    throw new Exception("Test");
}

try {
    test();
} catch (Exception $e) {
    echo "Caught: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
Current data: {"items":["first","second"]}
Added cleanup item
Final data: {"items":["first","second","cleanup"]}
Caught: Test
