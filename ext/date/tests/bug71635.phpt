--TEST--
Bug #71635 (segfault in DatePeriod::getEndDate() when no end date has been set)
--FILE--
<?php
date_default_timezone_set('UTC');
$period = new DatePeriod(new DateTimeImmutable("now"), new DateInterval("P2Y4DT6H8M"), 2);

var_dump($period->getEndDate());
?>
--EXPECT--
NULL
