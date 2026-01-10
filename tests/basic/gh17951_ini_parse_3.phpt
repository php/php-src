--TEST--
GH-17951 INI Parse 3
--CREDITS--
Frederik Milling Pytlick (frederikpyt@protonmail.com)
--INI--
memory_limit=128M
max_memory_limit=256M
--FILE--
<?php
echo ini_get('max_memory_limit') . PHP_EOL;
echo ini_get('memory_limit') . PHP_EOL;
?>
--EXPECT--
256M
128M
