--TEST--
GH-17951 INI Parse 4
--CREDITS--
Frederik Milling Pytlick
frederikpyt@protonmail.com
--INI--
memory_limit=-1
max_memory_limit=128M
--FILE--
<?php
--EXPECTF--
Fatal error: memory_limit cannot be set to unlimited when max_memory_limit (%d bytes) is not unlimited in %s
