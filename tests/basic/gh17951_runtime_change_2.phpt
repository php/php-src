--TEST--
GH-17951 Runtime Change 2
--CREDITS--
Frederik Milling Pytlick (frederikpyt@protonmail.com)
--INI--
memory_limit=128M
max_memory_limit=512M
--FILE--
<?php
ini_set('memory_limit', '512M');
echo ini_get('memory_limit') . PHP_EOL;
?>
--EXPECT--
512M
