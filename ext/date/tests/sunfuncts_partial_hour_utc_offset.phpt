--TEST--
Incorrect timezone detection in date_sunrise() for partial-hour UTC offsets
--FILE--
<?php
date_default_timezone_set('Asia/Kolkata');
$timestamp = mktime(0, 0, 0, 2, 9, 2025);
$longitude = 22.57;
$latitude = 88.36;

echo @date_sunrise($timestamp, SUNFUNCS_RET_STRING, $longitude, $latitude), "\n";

$dt = new DateTime();
$dt->setTimestamp(date_sun_info($timestamp, $longitude, $latitude)['sunrise']);
echo $dt->format('H:i'), "\n";
?>
--EXPECT--
06:10
06:11
