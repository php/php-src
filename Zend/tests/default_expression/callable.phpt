--TEST--
Tests passing default to a callable object
--FILE--
<?php

class C {
    function __invoke($V = 'Alfa') {}
}
new C()($D = default);
var_dump($D);
?>
--EXPECT--
string(4) "Alfa"
