--TEST--
Tests passing default to anonymous class methods
--FILE--
<?php

$C = new class {
    function __invoke($V = 'Alfa') {}
    function F($V = 'Bravo') {}
};
$C($D = default);
var_dump($D);

$C->F($D = default);
var_dump($D);
?>
--EXPECT--
string(4) "Alfa"
string(5) "Bravo"
