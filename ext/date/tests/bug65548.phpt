--TEST--
Test for bug #65548: Comparison for DateTimeImmutable doesn't work
--CREDITS--
Boro Sitnikovski <buritomath@yahoo.com>
--INI--
date.timezone = UTC
--FILE--
<?php
$iToday = new DateTimeImmutable('today');
$iTomorrow = new DateTimeImmutable('tomorrow');

$mToday = new DateTime('today');
$mTomorrow = new DateTime('tomorrow');

var_dump($iToday < $iTomorrow);
var_dump($iToday == $iTomorrow);
var_dump($iToday > $iTomorrow);

var_dump($iToday == $mToday);
var_dump($iToday === $mToday);

var_dump($iToday < $mTomorrow);
var_dump($iToday == $mTomorrow);
var_dump($iToday > $mTomorrow);
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
