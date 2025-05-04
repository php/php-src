--TEST--
Multiline arrow function
--FILE--
<?php

function a($a) {
    $f = fn($a) => {
        echo "inside fn($a):\n";
        return $a;
    };

    $f($a);
    echo "inside a($a):\n";
}

a(12345);

?>
--EXPECT--
inside fn(12345):
inside a(12345):