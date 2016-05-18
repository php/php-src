--TEST--
Tests for DateTimeImmutable::createFrom
--INI--
date.timezone=Europe/London
--FILE--
<?php
$current = "2014-03-02 16:24:08";

$i = DateTimeImmutable::createFrom( date_create( $current ) );
var_dump( $i );

$i = DateTimeImmutable::createFrom( date_create_immutable( $current ) );
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
object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2014-03-02 16:24:08.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
