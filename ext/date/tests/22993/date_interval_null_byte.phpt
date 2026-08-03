--TEST--
GH-22993: date_interval_create_from_date_string() error message with embedded NUL byte
--FILE--
<?php

date_interval_create_from_date_string("foo\0bar");

?>
--EXPECTF--
Warning: date_interval_create_from_date_string(): Unknown or bad format (foo%0bar) at position 0 (f): %s
