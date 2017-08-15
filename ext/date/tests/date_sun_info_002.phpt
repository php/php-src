--TEST--
Test basic date_sun_info()
--FILE--
<?php
date_default_timezone_set('Europe/Oslo');
$sun_info = date_sun_info(strtotime("2007-04-13 08:31:15 UTC"), 59.21, 9.61);
foreach ($sun_info as $key => $elem )
{
	echo date( 'Y-m-d H:i:s T', $elem ),  " ", $key, "\n";
}
echo "Done\n";
?>
--EXPECTF--
2007-04-13 06:13:31 CEST sunrise
2007-04-13 20:30:51 CEST sunset
2007-04-13 13:22:11 CEST transit
2007-04-13 05:29:22 CEST civil_twilight_begin
2007-04-13 21:15:00 CEST civil_twilight_end
2007-04-13 04:31:43 CEST nautical_twilight_begin
2007-04-13 22:12:39 CEST nautical_twilight_end
2007-04-13 03:17:01 CEST astronomical_twilight_begin
2007-04-13 23:27:21 CEST astronomical_twilight_end
Done
