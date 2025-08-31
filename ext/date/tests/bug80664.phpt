--TEST--
Bug #80664 (DateTime objects behave incorrectly around DST transition)
--FILE--
<?php
$dt = new DateTime('@1604215800');
$dt->setTimezone(new DateTimeZone('America/Boise'));
echo "{$dt->format('Y-m-d H:i:s T')} | {$dt->getTimestamp()}\n";
$dt->add(new DateInterval('PT1H'));
echo "{$dt->format('Y-m-d H:i:s T')} | {$dt->getTimestamp()}\n";
$dt->add(new DateInterval('PT1H'));
echo "{$dt->format('Y-m-d H:i:s T')} | {$dt->getTimestamp()}\n";
$dt->add(new DateInterval('PT1M'));
echo "{$dt->format('Y-m-d H:i:s T')} | {$dt->getTimestamp()}\n\n";

$dt = new DateTime('@1604219400');
$dt->setTimezone(new DateTimeZone('UTC'));
echo "{$dt->format('Y-m-d H:i:s T')} | {$dt->getTimestamp()}\r\n";
$dt->setTimezone(new DateTimeZone('America/Boise'));
echo "{$dt->format('Y-m-d H:i:s T')} | {$dt->getTimestamp()}\r\n";
?>
--EXPECT--
2020-11-01 01:30:00 MDT | 1604215800
2020-11-01 01:30:00 MST | 1604219400
2020-11-01 02:30:00 MST | 1604223000
2020-11-01 02:31:00 MST | 1604223060

2020-11-01 08:30:00 UTC | 1604219400
2020-11-01 01:30:00 MST | 1604219400
