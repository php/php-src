--TEST--
mktime()
--FILE--
<?php
$timezones = array(
    'GMT',
    'GST-1GDT'
);

foreach($timezones as $timezone)
{
    putenv('TZ='.$timezone);

    /* status of daylight saving time unknown */
    var_dump(mktime(0, 0, 0, 1, 1, 2002));
    /* status of daylight saving time unknown */
    var_dump(mktime(0, 0, 0, 1, 1, 2002, -1));
    /* daylight saving time is not in affect */
    var_dump(mktime(0, 0, 0, 1, 1, 2002, 0));
    /* daylight saving time is in affect */
    var_dump(mktime(0, 0, 0, 1, 1, 2002, 1));

    /* status of daylight saving time unknown */
    var_dump(mktime(0, 0, 0, 7, 1, 2002));
    /* status of daylight saving time unknown */
    var_dump(mktime(0, 0, 0, 7, 1, 2002, -1));
    /* daylight saving time is not in affect */
    var_dump(mktime(0, 0, 0, 7, 1, 2002, 0));
    /* daylight saving time is in affect */
    var_dump(mktime(0, 0, 0, 7, 1, 2002, 1));
}
?>
--EXPECT--
int(1009843200)
int(1009843200)
int(1009843200)
int(1009843200)
int(1025481600)
int(1025481600)
int(1025481600)
int(1025481600)
int(1009839600)
int(1009839600)
int(1009839600)
int(1009836000)
int(1025474400)
int(1025474400)
int(1025478000)
int(1025474400)
