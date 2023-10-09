--TEST--
Test for bug GH-8964: DateTime object comparison after applying delta less than 1 second
--INI--
date.timezone=UTC
--FILE--
<?php
$actual = new DateTimeImmutable("2022-07-21 15:00:10");
$delta    = new \DateInterval(sprintf('PT%dS', 0));
$delta->f = 0.9;

$expectedLower = $actual->sub($delta);
$expectedUpper = $actual->add($delta);

echo $expectedLower->format( 'H:i:s.u U' ), "\n";
echo $actual       ->format( 'H:i:s.u U' ), "\n";
echo $expectedUpper->format( 'H:i:s.u U' ), "\n";

var_dump($actual < $expectedLower, $actual > $expectedUpper);
?>
--EXPECTF--
15:00:09.100000 1658415609
15:00:10.000000 1658415610
15:00:10.900000 1658415610
bool(false)
bool(false)
