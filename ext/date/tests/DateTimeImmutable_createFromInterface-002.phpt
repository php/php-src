--TEST--
Tests for DateTimeImmutable::createFromInterface inheritance
--INI--
date.timezone=Europe/London
--FILE--
<?php
class MyDateTimeImmutable extends DateTimeImmutable {}

$current = "2014-03-02 16:24:08";

$i = MyDateTimeImmutable::createFromInterface( date_create( $current ) );
var_dump( $i );

$i = MyDateTimeImmutable::createFromInterface( date_create_immutable( $current ) );
var_dump( $i );

$current = "2019-12-16 15:06:46 CET";

$i = MyDateTimeImmutable::createFromInterface( date_create( $current ) );
var_dump( $i );

$i = MyDateTimeImmutable::createFromInterface( date_create_immutable( $current ) );
var_dump( $i );

$current = "2019-12-16 15:08:20 +0100";

$i = MyDateTimeImmutable::createFromInterface( date_create( $current ) );
var_dump( $i );

$i = MyDateTimeImmutable::createFromInterface( date_create_immutable( $current ) );
var_dump( $i );
?>
--EXPECTF--
object(MyDateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2014-03-02 16:24:08.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
object(MyDateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2014-03-02 16:24:08.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
object(MyDateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2019-12-16 15:06:46.000000"
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(3) "CET"
}
object(MyDateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2019-12-16 15:06:46.000000"
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(3) "CET"
}
object(MyDateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2019-12-16 15:08:20.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+01:00"
}
object(MyDateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2019-12-16 15:08:20.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+01:00"
}
