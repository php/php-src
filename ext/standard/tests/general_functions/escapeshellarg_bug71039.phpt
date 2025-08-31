--TEST--
Test escapeshellarg() string with \0 bytes
--FILE--
<?php
escapeshellarg("hello\0world");

?>
===DONE===
--EXPECTF--
Fatal error: Uncaught ValueError: escapeshellarg(): Argument #1 ($arg) must not contain any null bytes in %s:%d
Stack trace:
#0 %s(%d): escapeshellarg('hello\x00world')
#1 {main}
  thrown in %s on line %d
