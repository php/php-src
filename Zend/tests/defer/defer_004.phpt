--TEST--
Defer with variable access
--FILE--
<?php
function test_defer_with_variables() {
    $file = "test.txt";
    echo "Opening $file\n";

    defer {
        echo "Closing $file\n";
    }

    echo "Processing\n";
}

test_defer_with_variables();
?>
--EXPECT--
Opening test.txt
Processing
Closing test.txt
