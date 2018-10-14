--TEST--
Test return type and value, as well as basic behaviour, of ob_get_clean()
--FILE--
<?php
/*
 * proto bool ob_get_clean(void)
 * Function is implemented in main/output.c
*/

var_dump(ob_get_clean());

ob_start();
echo "Hello World";
var_dump(ob_get_clean());
?>
--EXPECTF--
bool(false)
string(11) "Hello World"
