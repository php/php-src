--TEST--
Bug #54316 (DateTime::createFromFormat does not handle trailing '|' correctly)
--INI--
date.timezone=UTC
--FILE--
<?php
$dt = DateTime::createFromFormat('Y-m-d|', '2011-02-02');
var_dump($dt);

$dt = DateTime::createFromFormat('Y-m-d!', '2011-02-02');
var_dump($dt);
?>
--EXPECT--
object(DateTime)#1 (3) {
  ["date"]=>
  string(26) "2011-02-02 00:00:00.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
object(DateTime)#2 (3) {
  ["date"]=>
  string(26) "1970-01-01 00:00:00.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
