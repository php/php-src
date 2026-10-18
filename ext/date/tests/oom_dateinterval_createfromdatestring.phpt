--TEST--
DateInterval::createFromDateString() doesn't reach hard limit
--INI--
memory_limit=64M
--FILE--
<?php
var_dump(DateInterval::createFromDateString(str_repeat("\x01", 4 * 1024 * 1024)));
?>
--EXPECTF--
Warning: DateInterval::createFromDateString(): Unknown or bad format (%s) at position 0 (%s): Unexpected character in %s on line %d
bool(false)
