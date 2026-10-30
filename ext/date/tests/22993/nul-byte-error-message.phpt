--TEST--
GH-22993: DateTimeImmutable rejects embedded NUL byte
--FILE--
<?php

new DateTimeImmutable("foo\0bar");

?>
--EXPECTF--
Fatal error: Uncaught ValueError: DateTimeImmutable::__construct(): Argument #1 ($datetime) must not contain any null bytes in %s:%d
Stack trace:
#0 %s(%d): DateTimeImmutable->__construct('foo\x00bar')
#1 {main}
  thrown in %s on line %d