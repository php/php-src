--TEST--
mktime() [3] (64-bit)
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip 64-bit only"); ?>
--INI--
error_reporting=2047
--FILE--
<?php
$tzs = array("America/Toronto", "Europe/Oslo");
$years = array(0, 69, 70, 71, 99, 100, 101, 105, 110, 1900, 1901, 1902, 1999, 2000, 2001);

foreach ($tzs as $tz) {
    echo $tz, "\n";
    date_default_timezone_set($tz);
    foreach ($years as $year) {
        printf("Y: %4d - ", $year);
        $ret = mktime(1, 1, 1, 1, 1, $year);
        if ($ret == FALSE) {
            echo "out of range\n";
        } else {
            echo date("F ".DATE_ATOM, $ret), "\n";
        }
    }
    echo "\n";
}
?>
--EXPECTF--
America/Toronto
Y:    0 - January 2000-01-01T01:01:01-05:00
Y:   69 - January 2069-01-01T01:01:01-05:00
Y:   70 - January 1970-01-01T01:01:01-05:00
Y:   71 - January 1971-01-01T01:01:01-05:00
Y:   99 - January 1999-01-01T01:01:01-05:00
Y:  100 - January 2000-01-01T01:01:01-05:00
Y:  101 - January 0101-01-01T01:01:01-05:17
Y:  105 - January 0105-01-01T01:01:01-05:17
Y:  110 - January 0110-01-01T01:01:01-05:17
Y: 1900 - January 1900-01-01T01:01:01-05:00
Y: 1901 - January 1901-01-01T01:01:01-05:00
Y: 1902 - January 1902-01-01T01:01:01-05:00
Y: 1999 - January 1999-01-01T01:01:01-05:00
Y: 2000 - January 2000-01-01T01:01:01-05:00
Y: 2001 - January 2001-01-01T01:01:01-05:00

Europe/Oslo
Y:    0 - January 2000-01-01T01:01:01+01:00
Y:   69 - January 2069-01-01T01:01:01+01:00
Y:   70 - January 1970-01-01T01:01:01+01:00
Y:   71 - January 1971-01-01T01:01:01+01:00
Y:   99 - January 1999-01-01T01:01:01+01:00
Y:  100 - January 2000-01-01T01:01:01+01:00
Y:  101 - January 0101-01-01T01:01:01+00:43
Y:  105 - January 0105-01-01T01:01:01+00:43
Y:  110 - January 0110-01-01T01:01:01+00:43
Y: 1900 - January 1900-01-01T01:01:01+01:00
Y: 1901 - January 1901-01-01T01:01:01+01:00
Y: 1902 - January 1902-01-01T01:01:01+01:00
Y: 1999 - January 1999-01-01T01:01:01+01:00
Y: 2000 - January 2000-01-01T01:01:01+01:00
Y: 2001 - January 2001-01-01T01:01:01+01:00
