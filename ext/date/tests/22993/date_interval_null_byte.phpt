--TEST--
GH-22993: date_interval_create_from_date_string() rejects embedded NUL byte
--FILE--
<?php

date_interval_create_from_date_string("foo\0bar");

?>
--EXPECTF--
Fatal error: Uncaught ValueError: date_interval_create_from_date_string(): Argument #1 ($datetime) must not contain any null bytes in %s:%d
Stack trace:
#0 %s(%d): date_interval_create_from_date_string('foo\x00bar')
#1 {main}
  thrown in %s on line %d