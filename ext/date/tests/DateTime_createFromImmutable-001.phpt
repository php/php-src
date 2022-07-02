--TEST--
Tests for DateTime::createFromImmutable
--INI--
date.timezone=Europe/London
--FILE--
<?php
$current = "2014-03-02 16:24:08";
$i = date_create_immutable( $current );

$m = DateTime::createFromImmutable( $i );
var_dump( $m );

$m->modify('+ 1 hour');

var_dump( $i->format('Y-m-d H:i:s') === $current );

try {
    DateTime::createFromImmutable( date_create( $current ) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2014-03-02 16:24:08.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
bool(true)
DateTime::createFromImmutable(): Argument #1 ($object) must be of type DateTimeImmutable, DateTime given
