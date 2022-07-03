--TEST--
Passing undefined variable to named arg
--FILE--
<?php

function func1($arg) { var_dump($arg); }
func1(arg: $undef);
func2(arg: $undef);
function func2($arg) { var_dump($arg); }

?>
--EXPECTF--
Warning: Undefined variable $undef (This will become an error in PHP 9.0) in %s on line %d
NULL

Warning: Undefined variable $undef (This will become an error in PHP 9.0) in %s on line %d
NULL
