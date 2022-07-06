--TEST--
Bug #74057: wrong day when using "this week" in strtotime
--FILE--
<?php
echo date("D Y-m-d", strtotime("saturday this week", strtotime("Sun 2017-01-01")))."\n";
echo date("D Y-m-d", strtotime("saturday this week", strtotime("Mon 2017-01-02")))."\n";
echo date("D Y-m-d", strtotime("saturday this week", strtotime("Tue 2017-01-03")))."\n";
echo date("D Y-m-d", strtotime("saturday this week", strtotime("Wed 2017-01-04")))."\n";
echo date("D Y-m-d", strtotime("saturday this week", strtotime("Thu 2017-01-05")))."\n";
echo date("D Y-m-d", strtotime("saturday this week", strtotime("Fri 2017-01-06")))."\n";
echo date("D Y-m-d", strtotime("saturday this week", strtotime("Sat 2017-01-07")))."\n";
echo date("D Y-m-d", strtotime("saturday this week", strtotime("Sun 2017-01-08")))."\n";
echo date("D Y-m-d", strtotime("saturday this week", strtotime("Mon 2017-01-09")))."\n";
?>
--EXPECT--
Sat 2016-12-31
Sat 2017-01-07
Sat 2017-01-07
Sat 2017-01-07
Sat 2017-01-07
Sat 2017-01-07
Sat 2017-01-07
Sat 2017-01-07
Sat 2017-01-14
