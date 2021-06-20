--TEST--
Bug #73460 (Datetime add not realising it already applied DST change)
--FILE--
<?php
$date = new DateTime('2016-11-05 23:45:00', new DateTimeZone('America/New_York'));

foreach (range(1, 20) as $i) {
	echo $date->format('Y/m/d H:i e T'), "\n";

	$date->add(new DateInterval('PT15M'));
}
?>
--EXPECT--
2016/11/05 23:45 America/New_York EDT
2016/11/06 00:00 America/New_York EDT
2016/11/06 00:15 America/New_York EDT
2016/11/06 00:30 America/New_York EDT
2016/11/06 00:45 America/New_York EDT
2016/11/06 01:00 America/New_York EDT
2016/11/06 01:15 America/New_York EDT
2016/11/06 01:30 America/New_York EDT
2016/11/06 01:45 America/New_York EDT
2016/11/06 01:00 America/New_York EST
2016/11/06 01:15 America/New_York EST
2016/11/06 01:30 America/New_York EST
2016/11/06 01:45 America/New_York EST
2016/11/06 02:00 America/New_York EST
2016/11/06 02:15 America/New_York EST
2016/11/06 02:30 America/New_York EST
2016/11/06 02:45 America/New_York EST
2016/11/06 03:00 America/New_York EST
2016/11/06 03:15 America/New_York EST
2016/11/06 03:30 America/New_York EST
