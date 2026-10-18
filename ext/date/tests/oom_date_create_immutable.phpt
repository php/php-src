--TEST--
date_create_immutable() doesn't reach hard limit
--INI--
memory_limit=64M
--FILE--
<?php
var_dump(date_create_immutable(str_repeat("\x01", 4 * 1024 * 1024)));
?>
--EXPECT--
bool(false)
