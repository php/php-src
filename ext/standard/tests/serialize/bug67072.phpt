--TEST--
Bug #67072 Echoing unserialized "SplFileObject" crash
--FILE--
<?php
	echo unserialize('O:13:"SplFileObject":1:{s:9:"*filename";s:15:"/home/flag/flag";}');
?>
===DONE==
--EXPECTF--
Fatal error: Uncaught exception 'Exception' with message 'Unserialization of 'SplFileObject' is not allowed' in %sbug67072.php:2
Stack trace:
#0 %sbug67072.php(2): unserialize('O:13:"SplFileOb...')
#1 {main}
  thrown in %sbug67072.php on line 2
