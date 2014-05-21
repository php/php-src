--TEST--
Tests for DateTimeImmutable::createFromMutable.
--INI--
date.timezone=Europe/London
--FILE--
<?php
$current = "2014-03-02 16:24:08";

$i = DateTimeImmutable::createFromMutable( date_create( $current ) );
var_dump( $i );

$i = DateTimeImmutable::createFromMutable( date_create_immutable( $current ) );
var_dump( $i );
?>
--EXPECTF--
object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2014-03-02 16:24:08.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

Warning: DateTimeImmutable::createFromMutable() expects parameter 1 to be DateTime, object given in %stests%eDateTimeImmutable_createFromMutable.php on line %d
NULL
