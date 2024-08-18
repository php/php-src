--TEST--
Tests passing default to a user class method parameter
--FILE--
<?php

class C {
    function F($V = 'Alfa') {
        var_dump($V);
    }
}
new C()->f(default);
?>
--EXPECT--
string(4) "Alfa"
