--TEST--
Defer with mixed normal and exception paths
--FILE--
<?php
function testSuccess() {
    echo "Success function start\n";

    defer {
        echo "Success defer\n";
    }

    echo "Success function end\n";
    return "OK";
}

function testFailure() {
    echo "Failure function start\n";

    defer {
        echo "Failure defer\n";
    }

    throw new Exception("Failed");
}

$result = testSuccess();
echo "Result: $result\n";

try {
    testFailure();
} catch (Exception $e) {
    echo "Caught: " . $e->getMessage() . "\n";
}

echo "All done\n";
?>
--EXPECT--
Success function start
Success function end
Success defer
Result: OK
Failure function start
Failure defer
Caught: Failed
All done
