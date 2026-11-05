--TEST--
Test ignore_repeated_errors with error_clear_last() and error_get_last()
--INI--
ignore_repeated_errors=1
error_reporting=-1
html_errors=0
--FILE--
<?php
function warningA() {
    trigger_error('A', E_USER_WARNING);
}

warningA();
error_clear_last();
warningA();
var_dump(error_get_last()['message']);

error_reporting(E_ALL & ~E_USER_DEPRECATED);
warningA();
trigger_error('D', E_USER_DEPRECATED);
warningA();
var_dump(error_get_last()['message']);
?>
--EXPECTF--
Warning: A in %s on line %d

Warning: A in %s on line %d
string(1) "A"
string(1) "A"
