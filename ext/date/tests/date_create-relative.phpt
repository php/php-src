--TEST--
date_create() with large relative offsets
--FILE--
<?php

date_default_timezone_set('UTC');

if (!defined('PHP_INT_MIN')) {
	define('PHP_INT_MIN', intval(-PHP_INT_MAX - 1));
}

$base_time = '28 Feb 2008 12:00:00'; 

// Most offsets tested in strtotime-relative.phpt. These are tests for dates outside the 32-bit range.
$offsets = array(
	// around 10 leap year periods (4000 years) in days
	'1460000 days',
	'1460969 days',
	'1460970 days',
	'1460971 days',
	'1462970 days',
	
	// around 1 leap year period in years
	'398 years',
	'399 years',
	'400 years',
	'401 years',
	
	// around 40000 years
	'39755 years',
	'39999 years',
	'40000 years',
	'40001 years',
	'41010 years',
	
	// bigger than int (32-bit)
	'10000000000 seconds',
	'10000000000 minutes',
	'10000000000 hours',
	'10000000000 days',
	'10000000000 months',
	'10000000000 years',
);

foreach ($offsets AS $offset) {
	foreach (array('+', '-') AS $direction) {
		$dt = date_create("$base_time $direction$offset");
		echo "$direction$offset: " . date_format($dt, DATE_ISO8601) . "\n";
	}
}

?>
--EXPECT--
+1460000 days: 6005-07-03T12:00:00+0000
-1460000 days: -1990-10-25T12:00:00+0000
+1460969 days: 6008-02-27T12:00:00+0000
-1460969 days: -1992-02-29T12:00:00+0000
+1460970 days: 6008-02-28T12:00:00+0000
-1460970 days: -1992-02-28T12:00:00+0000
+1460971 days: 6008-02-29T12:00:00+0000
-1460971 days: -1992-02-27T12:00:00+0000
+1462970 days: 6013-08-20T12:00:00+0000
-1462970 days: -1998-09-07T12:00:00+0000
+398 years: 2406-02-28T12:00:00+0000
-398 years: 1610-02-28T12:00:00+0000
+399 years: 2407-02-28T12:00:00+0000
-399 years: 1609-02-28T12:00:00+0000
+400 years: 2408-02-28T12:00:00+0000
-400 years: 1608-02-28T12:00:00+0000
+401 years: 2409-02-28T12:00:00+0000
-401 years: 1607-02-28T12:00:00+0000
+39755 years: 41763-02-28T12:00:00+0000
-39755 years: -37747-02-28T12:00:00+0000
+39999 years: 42007-02-28T12:00:00+0000
-39999 years: -37991-02-28T12:00:00+0000
+40000 years: 42008-02-28T12:00:00+0000
-40000 years: -37992-02-28T12:00:00+0000
+40001 years: 42009-02-28T12:00:00+0000
-40001 years: -37993-02-28T12:00:00+0000
+41010 years: 43018-02-28T12:00:00+0000
-41010 years: -39002-02-28T12:00:00+0000
+10000000000 seconds: 2325-01-18T05:46:40+0000
-10000000000 seconds: 1691-04-09T18:13:20+0000
+10000000000 minutes: 21021-05-27T22:40:00+0000
-10000000000 minutes: -17006-12-01T01:20:00+0000
+10000000000 hours: 1142802-09-30T04:00:00+0000
-10000000000 hours: -1138787-07-28T20:00:00+0000
+10000000000 days: 27381078-03-25T12:00:00+0000
-10000000000 days: -27377062-02-02T12:00:00+0000
+10000000000 months: 833335341-06-28T12:00:00+0000
-10000000000 months: -833331326-10-28T12:00:00+0000
+10000000000 years: 10000002008-02-28T12:00:00+0000
-10000000000 years: -9999997992-02-28T12:00:00+0000
