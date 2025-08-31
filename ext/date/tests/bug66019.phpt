--TEST--
Bug #66019 (DateTime object does not support short ISO 8601 time format - YYYY-MM-DDTHH)
--FILE--
<?php
$tz = new DateTimeZone("Europe/Amsterdam");
$dateObject = new DateTime( '2012-02-02T10', $tz );
echo $dateObject->format( 'j F Y H:i' );
?>
--EXPECT--
2 February 2012 10:00
