--TEST--
Test escapeshellarg() string with \0 bytes
--FILE--
<?php
escapeshellarg("hello\0world");

?>
===DONE===
--EXPECTF--
Fatal error: Uncaught TypeError: Input string contains NULL bytes in %s:%d
Stack trace:
#0 %s(%d): escapeshellarg('hello\x00world')
#1 {main}
  thrown in %s on line %d
