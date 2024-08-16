--TEST--
Test passing default argument to global user function.
--FILE--
<?php

function F($V = 'Alfa') {
    var_dump($V);
}
F(default);
?>
--EXPECT--
string(4) "Alfa"
