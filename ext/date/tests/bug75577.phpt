--TEST--
Test for bug #75577: DateTime::createFromFormat does not accept 'v' format specifier
--INI--
date.timezone=Europe/London
--FILE--
<?php
$d = new DateTime("2018-10-09 09:56:45.412000");
var_dump($s = $d->format(DateTime::RFC3339_EXTENDED));

$d2 = DateTime::createFromFormat(DateTime::RFC3339_EXTENDED, $s);
var_dump($d2);
?>
--EXPECTF--
string(29) "2018-10-09T09:56:45.412+01:00"
object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2018-10-09 09:56:45.412000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+01:00"
}
