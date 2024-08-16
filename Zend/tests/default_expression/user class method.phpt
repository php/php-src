--TEST--
Test passing default argument to user class method.
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
