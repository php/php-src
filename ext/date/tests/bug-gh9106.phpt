--TEST--
Test for bug GH-9601: DateInterval 1.5s added to DateTimeInterface is rounded down since PHP 8.1.0
--INI--
date.timezone=UTC
--FILE--
<?php

$start = new \DateTimeImmutable("2020-01-01 00:00:00 UTC");

$oneAndHalfSec = new \DateInterval("PT1S");
$oneAndHalfSec->f = 0.5;

$t1 = $start->add($oneAndHalfSec);
$t2 = $t1->add($oneAndHalfSec);
$t3 = $t2->add($oneAndHalfSec);
$t4 = $t3->add($oneAndHalfSec);

var_dump($start->getTimestamp());
var_dump($t1->getTimestamp());
var_dump($t2->getTimestamp());
var_dump($t3->getTimestamp());
var_dump($t4->getTimestamp());
?>
--EXPECT--
int(1577836800)
int(1577836801)
int(1577836803)
int(1577836804)
int(1577836806)
