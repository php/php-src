--TEST--
Bug #44742 (timezone_offset_get() causes segmentation faults)
--FILE--
<?php
date_default_timezone_set('Europe/London');
$dates = array(
	"2008-04-11 00:00:00+0000",
	"2008-04-11 00:00:00+0200",
	"2008-04-11 00:00:00+0330",
	"2008-04-11 00:00:00-0500",
	"2008-04-11 00:00:00-1130",
	"2008-04-11 00:00:00 CEST",
	"2008-04-11 00:00:00 CET",
	"2008-04-11 00:00:00 UTC",
	"2008-04-11 00:00:00 America/New_York",
	"2008-04-11 00:00:00 Europe/Oslo",
	"2008-04-11 00:00:00 Asia/Singapore",
);
foreach ($dates as $date)
{
	$date = date_create($date);
	var_dump(timezone_offset_get(date_timezone_get($date), $date));
}
?>
--EXPECT--
int(0)
int(7200)
int(12600)
int(-18000)
int(-41400)
int(7200)
int(3600)
int(0)
int(-14400)
int(7200)
int(28800)
