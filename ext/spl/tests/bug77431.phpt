--TEST--
Bug #77431 (SplFileInfo::__construct() accepts NUL bytes)
--FILE--
<?php
new SplFileInfo("bad\0good");
?>
--EXPECTF--
Fatal error: Uncaught TypeError: SplFileInfo::__construct() expects argument #1 ($file_name) to be a valid path, string given in %s:%d
Stack trace:%A
