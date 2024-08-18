--TEST--
Tests passing default to a global user function parameter
--FILE--
<?php

function F($V = 'Alfa') {
    var_dump($V);
}
F(default);
?>
--EXPECT--
string(4) "Alfa"
