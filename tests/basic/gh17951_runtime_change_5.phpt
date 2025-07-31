--TEST--
GH-17951 Runtime Change 5
--CREDITS--
Frederik Milling Pytlick (frederikpyt@protonmail.com)
--INI--
memory_limit=128M
max_memory_limit=512M
--FILE--
<?php
var_dump(ini_set('max_memory_limit', '128M'));
var_dump(ini_set('max_memory_limit', '256M'));
var_dump(ini_set('max_memory_limit', '512M'));
var_dump(ini_set('max_memory_limit', '-1'));
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
