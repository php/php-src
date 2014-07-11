--TEST--
Test error operation of password_get_info()
--FILE--
<?php
//-=-=-=-
var_dump(password_get_info());
var_dump(password_get_info(array()));

echo "OK!";
?>
--EXPECTF--
Warning: password_get_info() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: password_get_info() expects parameter 1 to be string, array given in %s on line %d
NULL
OK!
