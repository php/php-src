--TEST--
GH-17951 Runtime Change 3
--CREDITS--
Frederik Milling Pytlick (frederikpyt@protonmail.com)
--INI--
memory_limit=128M
max_memory_limit=512M
--FILE--
<?php
ini_set('memory_limit', '1024M');
echo ini_get('memory_limit');
--EXPECTF--
Warning: Failed to set memory_limit to %d bytes. memory_limit (currently: %d bytes) cannot exceed max_memory_limit (%d bytes) in %s
128M
