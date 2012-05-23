--TEST--
Bug #62112: Regression in number_format() in PHP 5.4
--FILE--
<?php
var_dump(number_format(2000.1, 0, ".", "\x00"));
var_dump(number_format(0.1, 1, "\x00", ","));
?>
==DONE==
--EXPECTF--
string(4) "2000"
string(2) "01"
==DONE==
