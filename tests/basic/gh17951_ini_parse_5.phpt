--TEST--
GH-17951 INI Parse 5
--CREDITS--
Frederik Milling Pytlick
frederikpyt@protonmail.com
--INI--
memory_limit=256M
max_memory_limit=128M
--FILE--
<?php
echo ini_get('max_memory_limit') . PHP_EOL;
echo ini_get('memory_limit') . PHP_EOL;
--EXPECTF--
Fatal error: memory_limit (%d bytes) exceeds max_memory_limit (%d bytes) in %s
-1
256M
