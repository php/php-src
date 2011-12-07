--TEST--
Bug #54597 (incorrect years for DateTime objects created with 4-digit years)
--INI--
date.timezone=Europe/London
--FILE--
<?php
$tz = new DateTimeZone("Europe/Amsterdam");
$dateObject = new DateTime( 'January 0099', $tz );
echo $dateObject->format( 'Y' ), "\n";
$dateObject = new DateTime( 'January 1, 0099', $tz );
echo $dateObject->format( 'Y' ), "\n";
$dateObject = new DateTime( '0099-01', $tz );
echo $dateObject->format( 'Y' ), "\n";
?>
--EXPECT--
0099
0099
0099
