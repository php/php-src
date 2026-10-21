--TEST--
date_modify() doesn't reach hard limit
--INI--
memory_limit=64M
--FILE--
<?php
var_dump(date_modify(new DateTime(), str_repeat("\x01", 4 * 1024 * 1024)));
?>
--EXPECTF--
Warning: date_modify(): Failed to parse time string (%s) at position 0 (%s): Unexpected character in %s on line %d
bool(false)
