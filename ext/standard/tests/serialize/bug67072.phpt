--TEST--
Bug #67072 Echoing unserialized "SplFileObject" crash
--FILE--
<?php
echo unserialize('O:13:"SplFileObject":1:{s:9:"*filename";s:15:"/home/flag/flag";}');
?>
===DONE==
--EXPECTF--
Fatal error: Uncaught Exception: Unserialization of 'SplFileObject' is not allowed in %s:%d
Stack trace:
#0 %s(%d): unserialize('O:13:"SplFileOb...')
#1 {main}
  thrown in %s on line %d
