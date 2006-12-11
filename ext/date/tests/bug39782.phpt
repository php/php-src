--TEST--
Bug #39782 (setTime() on a DateTime constructed with a Weekday yields incorrect results)
--FILE--
<?php
date_default_timezone_set("GMT");

$dttTest = new DateTime('Dec 10 2006 Next Wednesday');
echo $dttTest->format('D M j Y - H:i:s') . "\n";

$dttTest->setTime(12, 0, 0);
echo $dttTest->format('D M j Y - H:i:s') . "\n";

$dttTest->setTime(12, 0, 0);
echo $dttTest->format('D M j Y - H:i:s') . "\n";
?>
--EXPECT--
Wed Dec 13 2006 - 00:00:00
Wed Dec 13 2006 - 12:00:00
Wed Dec 13 2006 - 12:00:00