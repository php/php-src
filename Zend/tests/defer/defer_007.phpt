--TEST--
Defer with references and modifications
--FILE--
<?php
function test() {
    $counter = 0;
    echo "Initial counter: $counter\n";

    defer {
        global $counter;
        $counter += 10;
        echo "Defer 1: counter = $counter\n";
    }

    defer {
        global $counter;
        $counter += 5;
        echo "Defer 2: counter = $counter\n";
    }

    $counter = 100;
    echo "Modified counter: $counter\n";
}

$counter = 0;
test();
echo "Final counter: $counter\n";
?>
--EXPECT--
Initial counter: 0
Modified counter: 100
Defer 2: counter = 5
Defer 1: counter = 15
Final counter: 15
