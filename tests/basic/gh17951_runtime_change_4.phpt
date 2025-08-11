--TEST--
GH-17951 Runtime Change 4
--CREDITS--
Frederik Milling Pytlick (frederikpyt@protonmail.com)
--INI--
memory_limit=128M
max_memory_limit=512M
--FILE--
<?php
ini_set('memory_limit', '-1');
echo ini_get('memory_limit');
?>
--EXPECT--
512M
