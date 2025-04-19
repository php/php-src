--TEST--
GH-17951 Runtime Change 4
--CREDITS--
Frederik Milling Pytlick
frederikpyt@protonmail.com
--INI--
memory_limit=128M
max_memory_limit=512M
--FILE--
<?php
ini_set('memory_limit', '-1');
echo ini_get('memory_limit');
--EXPECTF--
Warning: Failed to set memory_limit to unlimited. memory_limit (currently: %d bytes) cannot be set to unlimited if max_memory_limit (%d bytes) is not unlimited in %s
128M
