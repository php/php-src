--TEST--
Tests passing multiple default arguments to global user function parameters
--FILE--
<?php

function F($V = 'Alfa', $V2 = 'Bravo') {
    var_dump($V);
    var_dump($V2);
}
F(default, default);
?>
--EXPECT--
string(4) "Alfa"
string(5) "Bravo"
