--TEST--
Defer with conditional execution and goto
--FILE--
<?php
function test($shouldFail) {
    echo "Test start with shouldFail=$shouldFail\n";

    defer {
        echo "Cleanup defer\n";
    }

    if ($shouldFail) {
        goto error;
    }

    echo "Normal path\n";
    return "success";

error:
    echo "Error path\n";
    throw new Exception("Failed");
}

$result = test(false);
echo "Result: $result\n";

try {
    test(true);
} catch (Exception $e) {
    echo "Caught: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
Test start with shouldFail=
Normal path
Cleanup defer
Result: success
Test start with shouldFail=1
Error path
Cleanup defer
Caught: Failed
