--TEST--
GH-17951 INI Parse 5
--CREDITS--
Frederik Milling Pytlick (frederikpyt@protonmail.com)
--INI--
memory_limit=256M
max_memory_limit=128M
--FILE--
<?php
echo ini_get('max_memory_limit') . PHP_EOL;
echo ini_get('memory_limit') . PHP_EOL;
?>
--EXPECT--
Warning: Failed to set memory_limit to 268435456 bytes. Setting to max_memory_limit instead (currently: 134217728 bytes) in Unknown on line 0
128M
128M
