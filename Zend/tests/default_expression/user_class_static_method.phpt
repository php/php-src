--TEST--
Tests passing default to a user class static method parameter
--FILE--
<?php

class C {
    static function F($V = 'Alfa') {
        var_dump($V);
    }
}
C::f(default);
?>
--EXPECT--
string(4) "Alfa"
