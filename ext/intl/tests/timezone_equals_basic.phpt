--TEST--
IntlTimeZone equals handler: basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$tz1 = intltz_create_time_zone('Europe/Lisbon');
$tz2 = intltz_create_time_zone('Europe/Lisbon');
echo "Comparison to self:\n";
var_dump($tz1 == $tz1);
echo "Comparison to equal instance:\n";
var_dump($tz1 == $tz2);
echo "Comparison to equivalent instance:\n";
var_dump($tz1 == intltz_create_time_zone('Portugal'));
echo "Comparison to GMT:\n";
var_dump($tz1 == intltz_get_gmt());

?>
==DONE==
--EXPECT--
Comparison to self:
bool(true)
Comparison to equal instance:
bool(true)
Comparison to equivalent instance:
bool(false)
Comparison to GMT:
bool(false)
==DONE==