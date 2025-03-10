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
--EXPECTF--
Fatal error: memory_limit (%d bytes) exceeds max_memory_limit (%d bytes) in %s
