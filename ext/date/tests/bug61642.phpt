--TEST--
Bug #61642 (modify("+5 weekdays") returns Sunday)
--INI--
date.timezone=UTC
--FILE--
<?php
// ±5 and ±10 (and any non-zero multiple of 5) is broken, but everything else
// should already work correctly.
$weekdays = range(-11, 11);
$dates = array('2012-03-29', '2012-03-30', '2012-03-31', '2012-04-01', '2012-04-02', '2012-04-03', '2012-04-04', '2012-04-05');

$header = array();

foreach ($dates as $startdate) {
	$date = new DateTime($startdate);

	$header[] = $date->format('Y-m-d D');
}

echo '###  ', implode('  ', $header), "\n\n";

foreach ($weekdays as $days) {
	$line = array();

	printf('%+3d  ', $days);

	foreach ($dates as $startdate) {
		$date = new DateTime($startdate);
		$date->modify("{$days} weekdays");

		$line[] = $date->format('Y-m-d D');
	}

	echo implode('  ', $line), "\n";
}
?>
--EXPECT--
###  2012-03-29 Thu  2012-03-30 Fri  2012-03-31 Sat  2012-04-01 Sun  2012-04-02 Mon  2012-04-03 Tue  2012-04-04 Wed  2012-04-05 Thu

-11  2012-03-14 Wed  2012-03-15 Thu  2012-03-16 Fri  2012-03-16 Fri  2012-03-16 Fri  2012-03-19 Mon  2012-03-20 Tue  2012-03-21 Wed
-10  2012-03-15 Thu  2012-03-16 Fri  2012-03-19 Mon  2012-03-19 Mon  2012-03-19 Mon  2012-03-20 Tue  2012-03-21 Wed  2012-03-22 Thu
 -9  2012-03-16 Fri  2012-03-19 Mon  2012-03-20 Tue  2012-03-20 Tue  2012-03-20 Tue  2012-03-21 Wed  2012-03-22 Thu  2012-03-23 Fri
 -8  2012-03-19 Mon  2012-03-20 Tue  2012-03-21 Wed  2012-03-21 Wed  2012-03-21 Wed  2012-03-22 Thu  2012-03-23 Fri  2012-03-26 Mon
 -7  2012-03-20 Tue  2012-03-21 Wed  2012-03-22 Thu  2012-03-22 Thu  2012-03-22 Thu  2012-03-23 Fri  2012-03-26 Mon  2012-03-27 Tue
 -6  2012-03-21 Wed  2012-03-22 Thu  2012-03-23 Fri  2012-03-23 Fri  2012-03-23 Fri  2012-03-26 Mon  2012-03-27 Tue  2012-03-28 Wed
 -5  2012-03-22 Thu  2012-03-23 Fri  2012-03-26 Mon  2012-03-26 Mon  2012-03-26 Mon  2012-03-27 Tue  2012-03-28 Wed  2012-03-29 Thu
 -4  2012-03-23 Fri  2012-03-26 Mon  2012-03-27 Tue  2012-03-27 Tue  2012-03-27 Tue  2012-03-28 Wed  2012-03-29 Thu  2012-03-30 Fri
 -3  2012-03-26 Mon  2012-03-27 Tue  2012-03-28 Wed  2012-03-28 Wed  2012-03-28 Wed  2012-03-29 Thu  2012-03-30 Fri  2012-04-02 Mon
 -2  2012-03-27 Tue  2012-03-28 Wed  2012-03-29 Thu  2012-03-29 Thu  2012-03-29 Thu  2012-03-30 Fri  2012-04-02 Mon  2012-04-03 Tue
 -1  2012-03-28 Wed  2012-03-29 Thu  2012-03-30 Fri  2012-03-30 Fri  2012-03-30 Fri  2012-04-02 Mon  2012-04-03 Tue  2012-04-04 Wed
 +0  2012-03-29 Thu  2012-03-30 Fri  2012-04-02 Mon  2012-04-02 Mon  2012-04-02 Mon  2012-04-03 Tue  2012-04-04 Wed  2012-04-05 Thu
 +1  2012-03-30 Fri  2012-04-02 Mon  2012-04-02 Mon  2012-04-02 Mon  2012-04-03 Tue  2012-04-04 Wed  2012-04-05 Thu  2012-04-06 Fri
 +2  2012-04-02 Mon  2012-04-03 Tue  2012-04-03 Tue  2012-04-03 Tue  2012-04-04 Wed  2012-04-05 Thu  2012-04-06 Fri  2012-04-09 Mon
 +3  2012-04-03 Tue  2012-04-04 Wed  2012-04-04 Wed  2012-04-04 Wed  2012-04-05 Thu  2012-04-06 Fri  2012-04-09 Mon  2012-04-10 Tue
 +4  2012-04-04 Wed  2012-04-05 Thu  2012-04-05 Thu  2012-04-05 Thu  2012-04-06 Fri  2012-04-09 Mon  2012-04-10 Tue  2012-04-11 Wed
 +5  2012-04-05 Thu  2012-04-06 Fri  2012-04-06 Fri  2012-04-06 Fri  2012-04-09 Mon  2012-04-10 Tue  2012-04-11 Wed  2012-04-12 Thu
 +6  2012-04-06 Fri  2012-04-09 Mon  2012-04-09 Mon  2012-04-09 Mon  2012-04-10 Tue  2012-04-11 Wed  2012-04-12 Thu  2012-04-13 Fri
 +7  2012-04-09 Mon  2012-04-10 Tue  2012-04-10 Tue  2012-04-10 Tue  2012-04-11 Wed  2012-04-12 Thu  2012-04-13 Fri  2012-04-16 Mon
 +8  2012-04-10 Tue  2012-04-11 Wed  2012-04-11 Wed  2012-04-11 Wed  2012-04-12 Thu  2012-04-13 Fri  2012-04-16 Mon  2012-04-17 Tue
 +9  2012-04-11 Wed  2012-04-12 Thu  2012-04-12 Thu  2012-04-12 Thu  2012-04-13 Fri  2012-04-16 Mon  2012-04-17 Tue  2012-04-18 Wed
+10  2012-04-12 Thu  2012-04-13 Fri  2012-04-13 Fri  2012-04-13 Fri  2012-04-16 Mon  2012-04-17 Tue  2012-04-18 Wed  2012-04-19 Thu
+11  2012-04-13 Fri  2012-04-16 Mon  2012-04-16 Mon  2012-04-16 Mon  2012-04-17 Tue  2012-04-18 Wed  2012-04-19 Thu  2012-04-20 Fri
