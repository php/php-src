--TEST--
Bug #77431 (SplFileInfo::__construct() accepts NUL bytes)
--FILE--
<?php
new SplFileInfo("bad\0good");
?>
--EXPECTF--
Fatal error: Uncaught TypeError: SplFileInfo::__construct(): Argument #1 ($file_name) must be a valid path, string given in %s:%d
Stack trace:
#0 %s(%d): SplFileInfo->__construct('bad\x00good')
#1 {main}
  thrown in %s on line %d
