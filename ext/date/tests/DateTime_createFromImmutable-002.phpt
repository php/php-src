--TEST--
Tests for inherited DateTime::createFromImmutable
--INI--
date.timezone=Europe/London
--FILE--
<?php
class MyDateTime extends DateTime {}

$current = "2014-03-02 16:24:08";
$i = date_create_immutable( $current );

$m = MyDateTime::createFromImmutable( $i );
var_dump( $m );

$m->modify('+ 1 hour');

var_dump( $i->format('Y-m-d H:i:s') === $current );

try {
    MyDateTime::createFromImmutable( date_create( $current ) );
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
object(MyDateTime)#%d (3) {
  ["date"]=>
  string(26) "2014-03-02 16:24:08.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
bool(true)
TypeError: DateTime::createFromImmutable(): Argument #1 ($object) must be of type DateTimeImmutable, DateTime given
