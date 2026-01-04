--TEST--
Multiple defers execute in LIFO order
--FILE--
<?php
function test_multiple_defers() {
    echo "Start\n";

    defer {
        echo "Defer 1\n";
    }

    echo "Middle\n";

    defer {
        echo "Defer 2\n";
    }

    echo "End\n";
}

test_multiple_defers();
?>
--EXPECT--
Start
Middle
End
Defer 2
Defer 1
