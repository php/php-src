--TEST--
strtotime() and mysql timestamps (32 bit)
--SKIPIF--
<?php if (PHP_INT_SIZE == 8) die("skip 32-bit only"); ?>
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
--EXPECTF--
string(31) "Fri, 23 May 1997 09:15:28 +0000"
string(31) "Sun, 31 Dec 2000 18:58:59 +0000"

Warning: strtotime(): Epoch doesn't fit in a PHP integer in %s on line %d
bool(false)
