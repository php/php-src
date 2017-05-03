--TEST--
Yield can be used with an operator on a sent value
--FILE--
<?php

function a () {
    echo yield * 3;
    echo yield + 3;
    echo yield - 3;
    echo yield / 3;
}

$a = a();
for ($i = 0; $i < 4; $i++) {
    $a->send(42);
    echo "\n";
}

?>
--EXPECT--
126
45
39
14
