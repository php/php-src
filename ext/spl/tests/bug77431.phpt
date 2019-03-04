--TEST--
Bug #77431 (SplFileInfo::__construct() accepts NUL bytes)
--FILE--
<?php
new SplFileInfo("bad\0good");
?>
--EXPECTF--
Fatal error: Uncaught TypeError: SplFileInfo::__construct() expects parameter 1 to be a valid path, string given in %s:%d
Stack trace:%A