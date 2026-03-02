--TEST--
Bug #65003 (Wrong date diff)
--FILE--
<?php
date_default_timezone_set("Europe/Moscow");

$datetime1 = new DateTime('13-03-01');
$datetime2 = new DateTime('13-04-01');

$datetime3 = new DateTime('13-03-02');
$datetime4 = new DateTime('13-04-02');

$interval = $datetime2->diff($datetime1);
echo $interval->format('%m month, %d days'), "\n"; //1 month, 3 days

$interval = $datetime4->diff($datetime3);
echo $interval->format('%m month, %d days'), "\n"; //1 month, 0 days
?>
--EXPECT--
1 month, 0 days
1 month, 0 days
