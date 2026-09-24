--TEST--
Setting memory_limit to -1 at runtime lifts the limit
--INI--
memory_limit=16M
--FILE--
<?php
var_dump(ini_set('memory_limit', '-1'));
var_dump(ini_get('memory_limit'));
var_dump(strlen(str_repeat('a', 32 * 1024 * 1024)));
?>
--EXPECT--
string(3) "16M"
string(2) "-1"
int(33554432)
