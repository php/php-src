--TEST--
mktime() [2]
--INI--
error_reporting=2047
--FILE--
<?php
$timezones = array(
    'UTC',
    'Europe/London'
);

foreach($timezones as $timezone)
{
    date_default_timezone_set($timezone);

    /* status of daylight saving time unknown */
    var_dump(mktime(0, 0, 0, 1, 1, 2002));
    /* status of daylight saving time unknown */
    var_dump(mktime(0, 0, 0, 1, 1, 2002, -1));
    /* daylight saving time is not in effect */
    var_dump(mktime(0, 0, 0, 1, 1, 2002, 0));
    /* daylight saving time is in effect */
    var_dump(mktime(0, 0, 0, 1, 1, 2002, 1));

    /* status of daylight saving time unknown */
    var_dump(mktime(0, 0, 0, 7, 1, 2002));
    /* status of daylight saving time unknown */
    var_dump(mktime(0, 0, 0, 7, 1, 2002, -1));
    /* daylight saving time is not in effect */
    var_dump(mktime(0, 0, 0, 7, 1, 2002, 0));
    /* daylight saving time is in effect */
    var_dump(mktime(0, 0, 0, 7, 1, 2002, 1));
}
?>
--EXPECTF--
int(1009843200)
int(1009843200)
int(1009843200)
int(%s)
int(1025481600)
int(1025481600)
int(1025481600)
int(%s)
int(1009843200)
int(1009843200)
int(1009843200)
int(1009839600)
int(1025478000)
int(1025478000)
int(1025481600)
int(1025478000)
