--TEST--
Defer with closures and use variables
--FILE--
<?php
function test() {
    $name = "Resource";
    $value = 42;

    echo "Opening $name\n";

    defer {
        echo "Closing $name with value $value\n";
    }

    $value = 100;
    echo "Processing with value $value\n";

    throw new Exception("Error during processing");
}

try {
    test();
} catch (Exception $e) {
    echo "Caught: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
Opening Resource
Processing with value 100
Closing Resource with value 100
Caught: Error during processing
