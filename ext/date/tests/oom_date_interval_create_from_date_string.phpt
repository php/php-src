--TEST--
date_interval_create_from_date_string() doesn't reach hard limit
--INI--
memory_limit=64M
--FILE--
<?php
var_dump(date_interval_create_from_date_string(str_repeat("\x01", 4 * 1024 * 1024)));
?>
--EXPECTF--
Warning: date_interval_create_from_date_string(): Unknown or bad format (%s) at position 0 (%s): Unexpected character in %s on line %d
bool(false)
