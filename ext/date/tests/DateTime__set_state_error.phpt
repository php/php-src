--TEST--
Test error on __set_state() method
--CREDITS--
Gabriel da Gama <gabriel@gabrielgama.com.br>
# PHPDublin PHPTestFest 2017
--FILE--
<?php
date_default_timezone_set('Europe/Dublin');
echo "*** Testing error on __set_state on DateTime objects ***\n";
var_dump(@DateTime::__set_state('error'));
?>
--EXPECTF--
*** Testing error on __set_state on DateTime objects ***
bool(false)
