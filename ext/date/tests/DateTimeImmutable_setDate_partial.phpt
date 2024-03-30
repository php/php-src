--TEST--
Test DateTimeImmutable::setDate() function : partial application
--FILE--
<?php
date_default_timezone_set('UTC');
$datetime = new DateTimeImmutable('1234-05-06 00:00:00');

echo $datetime->setDate()->format('c'), "\n";
echo $datetime->setDate(1)->format('c'), "\n";
echo $datetime->setDate(month: 2)->format('c'), "\n";
echo $datetime->setDate(3, day: 4)->format('c'), "\n";
?>
--EXPECT--
1234-05-06T00:00:00+00:00
0001-05-06T00:00:00+00:00
1234-02-06T00:00:00+00:00
0003-05-04T00:00:00+00:00
