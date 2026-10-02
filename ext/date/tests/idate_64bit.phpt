--TEST--
idate() with timestamps outside the 32bit range
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit int only");
?>
--INI--
date.timezone=UTC
--FILE--
<?php
$timestamps = [
    0,
    2147483647,     /* last timestamp a signed 32bit time_t holds */
    2147483648,     /* the first one it does not */
    4102444800,
    253402300799,
    -2147483649,    /* and below the negative bound */
];

foreach ($timestamps as $ts) {
    printf("%14d: %s-%s-%s %s  (date: %s)\n", $ts,
        idate('Y', $ts), idate('m', $ts), idate('d', $ts), idate('H', $ts),
        date('Y-n-j G', $ts));
}
?>
--EXPECT--
             0: 1970-1-1 0  (date: 1970-1-1 0)
    2147483647: 2038-1-19 3  (date: 2038-1-19 3)
    2147483648: 2038-1-19 3  (date: 2038-1-19 3)
    4102444800: 2100-1-1 0  (date: 2100-1-1 0)
  253402300799: 9999-12-31 23  (date: 9999-12-31 23)
   -2147483649: 1901-12-13 20  (date: 1901-12-13 20)
