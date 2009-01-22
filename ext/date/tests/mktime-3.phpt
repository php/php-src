--TEST--
mktime() [3] (32-bit)
--SKIPIF--
<?php echo PHP_INT_SIZE == 8 ? "skip 32-bit only" : "OK" ?>
--INI--
error_reporting=2047
--FILE--
<?php
$tzs = array("America/Toronto", "Europe/Oslo");
$years = array(0, 69, 70, 71, 99, 100, 105, 1900, 1901, 1902, 1999, 2000, 2001);

foreach ($tzs as $tz) {
	echo $tz, "\n";
	date_default_timezone_set($tz);
	foreach ($years as $year) {
		printf("Y: %4d - ", $year);
		$ret = mktime(1, 1, 1, 1, 1, $year);
		if ($ret == FALSE) {
			echo "out of range\n";
		} else {
			echo date("F ".DATE_ISO8601, $ret), "\n";
		}
	}
	echo "\n";
}
?>
===Done===
--EXPECT--
America/Toronto
Y:    0 - January 2000-01-01T01:01:01-0500
Y:   69 - out of range
Y:   70 - January 1970-01-01T01:01:01-0500
Y:   71 - January 1971-01-01T01:01:01-0500
Y:   99 - January 1999-01-01T01:01:01-0500
Y:  100 - January 2000-01-01T01:01:01-0500
Y:  105 - out of range
Y: 1900 - out of range
Y: 1901 - out of range
Y: 1902 - January 1902-01-01T01:01:01-0500
Y: 1999 - January 1999-01-01T01:01:01-0500
Y: 2000 - January 2000-01-01T01:01:01-0500
Y: 2001 - January 2001-01-01T01:01:01-0500

Europe/Oslo
Y:    0 - January 2000-01-01T01:01:01+0100
Y:   69 - out of range
Y:   70 - January 1970-01-01T01:01:01+0100
Y:   71 - January 1971-01-01T01:01:01+0100
Y:   99 - January 1999-01-01T01:01:01+0100
Y:  100 - January 2000-01-01T01:01:01+0100
Y:  105 - out of range
Y: 1900 - out of range
Y: 1901 - out of range
Y: 1902 - January 1902-01-01T01:01:01+0100
Y: 1999 - January 1999-01-01T01:01:01+0100
Y: 2000 - January 2000-01-01T01:01:01+0100
Y: 2001 - January 2001-01-01T01:01:01+0100

===Done===