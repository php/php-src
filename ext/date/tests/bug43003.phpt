--TEST--
Bug #43003 (Invalid timezone reported for DateTime objects constructed using a timestamp)
--FILE--
<?php
date_default_timezone_set('Europe/Oslo');

$oDateTest = new DateTime("@0", new DateTimeZone(date_default_timezone_get()));
echo $oDateTest->getTimezone()->getName().": " .  $oDateTest->format("Y-m-d H:i:s")."\n";

$oDateTest->setTimezone(new DateTimeZone("UTC"));
echo $oDateTest->getTimezone()->getName().": " .  $oDateTest->format("Y-m-d H:i:s")."\n";

$oDateTest->setTimezone(new DateTimeZone(date_default_timezone_get()));
echo $oDateTest->getTimezone()->getName().": " . $oDateTest->format("Y-m-d H:i:s")."\n";

$oDateTest = new DateTime("@0");
echo $oDateTest->getTimezone()->getName().": " .  $oDateTest->format("Y-m-d H:i:s")."\n";

$oDateTest->setTimezone( new DateTimeZone(date_default_timezone_get()));
echo $oDateTest->getTimezone()->getName().": " . $oDateTest->format("Y-m-d H:i:s")."\n";
?>
--EXPECT--
+00:00: 1970-01-01 00:00:00
UTC: 1970-01-01 00:00:00
Europe/Oslo: 1970-01-01 01:00:00
+00:00: 1970-01-01 00:00:00
Europe/Oslo: 1970-01-01 01:00:00
