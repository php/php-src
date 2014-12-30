--TEST--
DateTime::createFromFormat() does not allow NULL $timezone
--FILE--
<?php

date_default_timezone_set('America/Los_Angeles');
var_dump(DateTime::createFromFormat('Y/m/d H:i:s', '1995/06/08 12:34:56', null));
var_dump(DateTimeImmutable::createFromFormat('Y/m/d H:i:s', '1995/06/08 12:34:56', null));
--EXPECT--
object(DateTime)#1 (3) {
  ["date"]=>
  string(26) "1995-06-08 12:34:56.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(19) "America/Los_Angeles"
}
object(DateTimeImmutable)#1 (3) {
  ["date"]=>
  string(26) "1995-06-08 12:34:56.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(19) "America/Los_Angeles"
}
