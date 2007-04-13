--TEST--
Test basic date_sun_info()
--INI--
date.timezone=Europe/Oslo
--FILE--
<?php
$sun_info = date_sun_info(strtotime("2007-04-13 08:31:15 UTC"), 59.21, 9.61);
foreach ($sun_info as $key => $elem )
{
	echo date( 'Y-m-d H:i:s T', $elem ),  " ", $key, "\n";
}
echo "Done\n";
?>
--EXPECTF--
2007-04-13 06:12:19 CEST sunrise
2007-04-13 20:31:50 CEST sunset
2007-04-13 13:22:05 CEST transit
2007-04-13 05:28:03 CEST civil_twilight_begin
2007-04-13 21:16:06 CEST civil_twilight_end
2007-04-13 04:30:08 CEST nautical_twilight_begin
2007-04-13 22:14:01 CEST nautical_twilight_end
2007-04-13 03:14:36 CEST astronomical_twilight_begin
2007-04-13 23:29:33 CEST astronomical_twilight_end
Done
