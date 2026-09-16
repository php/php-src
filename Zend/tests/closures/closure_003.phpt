--TEST--
Closure 003: Lambda with lexical variables (local scope)
--FILE--
<?php

function run () {
    $x = 4;

    $lambda1 = function () use ($x) {
        echo "$x\n";
    };

    $lambda2 = function () use (&$x) {
        echo "$x\n";
    };

    $lambda1();
    $lambda2();
    $x++;
    $lambda1();
    $lambda2();
}

run();

echo "Done\n";
?>
--EXPECT--
4
4
4
5
Done
