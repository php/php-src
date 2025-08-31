--TEST--
Test escapeshellcmd() string with \0 bytes
--FILE--
<?php
escapeshellcmd("hello\0world");

?>
===DONE===
--EXPECTF--
Fatal error: Uncaught ValueError: escapeshellcmd(): Argument #1 ($command) must not contain any null bytes in %s:%d
Stack trace:
#0 %s(%d): escapeshellcmd('hello\x00world')
#1 {main}
  thrown in %s on line %d
