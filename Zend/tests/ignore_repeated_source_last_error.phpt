--TEST--
Repeated errors preserve error_get_last() when ignoring their source
--INI--
ignore_repeated_errors=1
ignore_repeated_source=1
error_reporting=-1
html_errors=0
--FILE--
<?php
trigger_error('S', E_USER_WARNING);
$last = error_get_last();
trigger_error('S', E_USER_NOTICE);
var_dump(error_get_last() === $last);

/* A different file must also be ignored, even with the same line number. */
eval(str_repeat("\n", $last['line'] - 1) . 'trigger_error("S", E_USER_NOTICE);');
var_dump(error_get_last() === $last);

/* A filtered error still updates error_get_last(). */
error_reporting(E_ALL & ~E_USER_DEPRECATED);
trigger_error('D', E_USER_DEPRECATED);
var_dump(error_get_last()['message']);
trigger_error('S', E_USER_WARNING);
var_dump(error_get_last()['type'] === E_USER_WARNING);
var_dump(error_get_last()['message']);
$last = error_get_last();
trigger_error('S', E_USER_NOTICE);
var_dump(error_get_last() === $last);

error_clear_last();
var_dump(error_get_last());
trigger_error('S', E_USER_NOTICE);
var_dump(error_get_last()['type'] === E_USER_NOTICE);

/* With source comparison enabled, a different line is a new error. */
ini_set('ignore_repeated_source', '0');
trigger_error('S', E_USER_WARNING);
var_dump(error_get_last()['type'] === E_USER_WARNING);
?>
--EXPECTF--
Warning: S in %s on line %d
bool(true)
bool(true)
string(1) "D"
bool(true)
string(1) "S"
bool(true)
NULL

Notice: S in %s on line %d
bool(true)

Warning: S in %s on line %d
bool(true)
