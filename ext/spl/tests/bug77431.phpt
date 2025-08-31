--TEST--
Bug #77431 (SplFileInfo::__construct() accepts NUL bytes)
--FILE--
<?php
new SplFileInfo("bad\0good");
?>
--EXPECTF--
Fatal error: Uncaught ValueError: SplFileInfo::__construct(): Argument #1 ($filename) must not contain any null bytes in %s:%d
Stack trace:
#0 %s(%d): SplFileInfo->__construct('bad\x00good')
#1 {main}
  thrown in %s on line %d
