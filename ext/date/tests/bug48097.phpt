--TEST--
Bug #48097 (date_timezone_set function produces wrong datetime result)
--INI--
date.timezone=UTC
--FILE--
<?php
$d = date_create( "Sun, 22 May 1955 02:00:00 +0200" );
var_dump( $d );
echo $d->format( DATE_ISO8601 ), "\n";
echo $d->format( 'U' ), "\n\n";

$d->setTimeZone( new DateTimeZone( 'Europe/Budapest' ) );
var_dump( $d );
echo $d->format( DATE_ISO8601 ), "\n\n";
echo $d->format( 'U' ), "\n\n";
?>
--EXPECT--
object(DateTime)#1 (3) {
  ["date"]=>
  string(26) "1955-05-22 02:00:00.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+02:00"
}
1955-05-22T02:00:00+0200
-461203200

object(DateTime)#1 (3) {
  ["date"]=>
  string(26) "1955-05-22 01:00:00.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(15) "Europe/Budapest"
}
1955-05-22T01:00:00+0100

-461203200
