--TEST--
Datetime diff with timezone
--FILE--
<?php

$tz    = new DateTimeZone('Asia/Shanghai');
$date1 = new DateTime('2020-03-01', $tz);
$date2 = new DateTime('2020-02-01', $tz);
var_dump($date1->diff($date2)->m);
echo "Done\n";
?>
--EXPECT--
int(1)
Done
