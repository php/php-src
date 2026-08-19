--TEST--
GH-22993: DateInterval::createFromDateString() rejects embedded NUL byte
--FILE--
<?php

DateInterval::createFromDateString("foo\0bar");

?>
--EXPECTF--
Fatal error: Uncaught ValueError: DateInterval::createFromDateString(): Argument #1 ($datetime) must not contain any null bytes in %s:%d
Stack trace:
#0 %s(%d): DateInterval::createFromDateString('foo\x00bar')
#1 {main}
  thrown in %s on line %d