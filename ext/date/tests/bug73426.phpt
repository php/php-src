--TEST--
Bug #73426 (createFromFormat with 'z' format char results in incorrect time)
--INI--
date.timezone=UTC
--FILE--
<?php
$date = '2016 12:00:00 15';
$format = 'Y H:i:s z';
var_dump(DateTime::createFromFormat($format, $date));

$date = '16 12:00:00 2016';
$format = 'z H:i:s Y';
var_dump(DateTime::createFromFormat($format, $date));
var_dump(DateTime::getLastErrors());
?>
--EXPECTF--
object(DateTime)#1 (3) {
  ["date"]=>
  string(26) "2016-01-16 12:00:00.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
bool(false)
array(4) {
  ["warning_count"]=>
  int(0)
  ["warnings"]=>
  array(0) {
  }
  ["error_count"]=>
  int(1)
  ["errors"]=>
  array(1) {
    [0]=>
    string(57) "A 'day of year' can only come after a year has been found"
  }
}
