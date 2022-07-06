--TEST--
Tests for inherited DateTimeImmutable::createFromMutable
--INI--
date.timezone=Europe/London
--FILE--
<?php
class MyDateTimeImmutable extends DateTimeImmutable {}

$current = "2014-03-02 16:24:08";

$i = MyDateTimeImmutable::createFromMutable( date_create( $current ) );
var_dump( $i );

try {
    MyDateTimeImmutable::createFromMutable( date_create_immutable( $current ) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
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
DateTimeImmutable::createFromMutable(): Argument #1 ($object) must be of type DateTime, DateTimeImmutable given
