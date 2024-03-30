--TEST--
Test DateTime::setDate() function : partial application
--FILE--
<?php
date_default_timezone_set('UTC');
$datetime = new DateTime('1234-05-06 00:00:00');

echo $datetime->setDate()->format('c'), "\n";
echo $datetime->setDate(1)->format('c'), "\n";
echo $datetime->setDate(month: 2)->format('c'), "\n";
echo $datetime->setDate(3, day: 4)->format('c'), "\n";
?>
--EXPECT--
1234-05-06T00:00:00+00:00
0001-05-06T00:00:00+00:00
0001-02-06T00:00:00+00:00
0003-02-04T00:00:00+00:00
