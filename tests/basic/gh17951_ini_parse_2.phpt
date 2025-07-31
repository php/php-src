--TEST--
GH-17951 INI Parse 2
--CREDITS--
Frederik Milling Pytlick (frederikpyt@protonmail.com)
--INI--
memory_limit=-1
max_memory_limit=-1
--FILE--
<?php
echo ini_get('max_memory_limit') . PHP_EOL;
echo ini_get('memory_limit') . PHP_EOL;
?>
--EXPECT--
-1
-1
