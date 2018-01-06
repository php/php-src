--TEST--
Tests for DateTime::createFromImmutable.
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

$m = DateTime::createFromImmutable( date_create( $current ) );
var_dump( $m );
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

Warning: DateTime::createFromImmutable() expects parameter 1 to be DateTimeImmutable, object given in %stests%eDateTime_createFromImmutable.php on line %d
NULL
