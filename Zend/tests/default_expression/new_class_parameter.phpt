--TEST--
Tests passing default to a parameter with a new class as the default
--FILE--
<?php

function F($V = new stdClass) {}
F($D = default);
var_dump($D);
?>
--EXPECT--
object(stdClass)#1 (0) {
}
