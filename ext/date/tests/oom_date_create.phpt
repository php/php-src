--TEST--
date_create() doesn't reach hard limit
--INI--
memory_limit=64M
--FILE--
<?php
var_dump(date_create(str_repeat("\x01", 4 * 1024 * 1024)));
?>
--EXPECT--
bool(false)
