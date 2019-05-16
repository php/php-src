--TEST--
strtotime() and mysql timestamps (64 bit)
--SKIPIF--
<?php echo PHP_INT_SIZE != 8 ? "skip 64-bit only" : "OK"; ?>
--FILE--
<?php
date_default_timezone_set('UTC');

/* Format: YYYYMMDDHHMMSS */
$d[] = '19970523091528';
$d[] = '20001231185859';
$d[] = '20800410101010'; // overflow..

foreach($d as $date) {
	$time = strtotime($date);

	if (is_integer($time)) {
		var_dump(date('r', $time));
	} else {
		var_dump($time);
	}
}
?>
--EXPECT--
string(31) "Fri, 23 May 1997 09:15:28 +0000"
string(31) "Sun, 31 Dec 2000 18:58:59 +0000"
string(31) "Wed, 10 Apr 2080 10:10:10 +0000"
