--TEST--
GH-17951 INI Parse 3
--CREDITS--
Frederik Milling Pytlick
frederikpyt@protonmail.com
--INI--
memory_limit=128M
max_memory_limit=256M
--FILE--
<?php
echo "OK";
--EXPECT--
OK
