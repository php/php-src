--TEST--
Call internal function with incorrect number of arguments
--FILE--
<?php
substr("foo");
array_diff([]);
--EXPECTF--
Warning: substr() expects at least 2 parameters, 1 given in %s

Warning: array_diff(): at least 2 parameters are required, 1 given in %s
