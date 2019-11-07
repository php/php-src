--TEST--
Test >> operator : numbers as strings, simple
--FILE--
<?php

error_reporting(E_ERROR);

var_dump("12" >> "0");
var_dump("34" >> "1");
var_dump("56" >> "2");

?>
--EXPECT--
int(12)
int(17)
int(14)
