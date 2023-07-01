--TEST--
Bug #46874 (DatePeriod not resetting after foreach loop)
--FILE--
<?php
$dp = DatePeriod::createFromISO8601String('R5/2008-03-01T13:00:00Z/P1Y2M10DT2H30M');

foreach ($dp as $date) {
    echo $date->format("Y-m-d H:i:s\n");
}

echo "\n";

// this should repeat the same range
foreach ($dp as $date) {
    echo $date->format("Y-m-d H:i:s\n");
}
?>
--EXPECT--
2008-03-01 13:00:00
2009-05-11 15:30:00
2010-07-21 18:00:00
2011-10-01 20:30:00
2012-12-11 23:00:00
2014-02-22 01:30:00

2008-03-01 13:00:00
2009-05-11 15:30:00
2010-07-21 18:00:00
2011-10-01 20:30:00
2012-12-11 23:00:00
2014-02-22 01:30:00
