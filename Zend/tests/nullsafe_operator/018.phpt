--TEST--
Test nullsafe on undefined variable
--FILE--
<?php

var_dump($foo?->bar);
var_dump($foo?->bar());
var_dump($foo);

?>
--EXPECTF--
Warning: Undefined variable $foo in %s.php on line 3
NULL

Warning: Undefined variable $foo in %s.php on line 4
NULL

Warning: Undefined variable $foo in %s.php on line 5
NULL
