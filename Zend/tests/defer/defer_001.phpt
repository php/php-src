--TEST--
Basic defer functionality
--FILE--
<?php
function test_basic_defer() {
    echo "Start\n";

    defer {
        echo "Deferred\n";
    }

    echo "End\n";
}

test_basic_defer();
?>
--EXPECT--
Start
End
Deferred
