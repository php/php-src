--TEST--
Bug #41599 (setTime() fails after modify() is used)
--FILE--
<?php
date_default_timezone_set('Europe/London');

$start = new DateTime('2008-01-17 last Monday');
echo $start->format('Y-m-d H:i:s'),PHP_EOL;
//good

$start->modify('Tuesday');
echo $start->format('Y-m-d H:i:s'),PHP_EOL;
//good

$start->setTime(4, 0, 0);
echo $start->format('Y-m-d H:i:s'),PHP_EOL;
//jumped to next Sunday

$start->setTime(8, 0, 0);
echo $start->format('Y-m-d H:i:s'),PHP_EOL;
//jumped to next Sunday again
?>
--EXPECT--
2008-01-14 00:00:00
2008-01-15 00:00:00
2008-01-15 04:00:00
2008-01-15 08:00:00
