--TEST--
Tests for DateTime::createFromImmutable.
--INI--
date.timezone=America/New_York
--FILE--
<?php
$current = "2015-03-05 07:00:16";

$i = DateTime::createFromImmutable(date_create_immutable($current));
var_dump($i);

$i = DateTime::createFromImmutable(date_create($current));
var_dump($i);
?>
--EXPECTF--
object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2015-03-05 07:00:16.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(16) "America/New_York"
}

Warning: DateTime::createFromImmutable() expects parameter 1 to be DateTimeImmutable, object given in %stests%eDateTime_createFromImmutable.php on line %d
NULL
