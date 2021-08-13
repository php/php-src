--TEST--
Bug #81106: Regression in 8.1: add() now truncate ->f
--FILE--
<?php
$dateInterval = new DateInterval('PT0S');
$dateInterval->f = 1.234;

echo (new DateTimeImmutable('2000-01-01 00:00:00'))->add($dateInterval)->format('Y-m-d H:i:s.u');
?>
--EXPECT--
2000-01-01 00:00:01.234000
