--TEST--
Bug #48097 (date_timezone_set function produces wrong datetime result)
--INI--
date.timezone=UTC
--FILE--
<?php
$d = date_create( "Mon, 23 May 1955 00:00:00 +0200" );
echo $d->format( DATE_ISO8601 ), "\n";
echo $d->format( 'U' ), "\n\n";

$d->setTimeZone( new DateTimeZone( 'Europe/Budapest' ) );
echo $d->format( DATE_ISO8601 ), "\n\n";
echo $d->format( 'U' ), "\n\n";
--EXPECT--
1955-05-23T00:00:00+0200
-461124000

1955-05-22T23:00:00+0100

-461124000
