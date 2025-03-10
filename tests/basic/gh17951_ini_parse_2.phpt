--TEST--
GH-17951 INI Parse 2
--CREDITS--
Frederik Milling Pytlick
frederikpyt@protonmail.com
--INI--
memory_limit=-1
max_memory_limit=-1
--FILE--
<?php
echo "OK";
--EXPECT--
OK
