--TEST--
Bug #24142 (round() problems)
--FILE--
<?php // $Id$ vim600:syn=php
$v = 0.005;
for ($i = 1; $i < 10; $i++) {
	echo "round({$v}, 2) -> ".round($v, 2)."\n";
	$v += 0.01;
}
?>
--EXPECT--
round(0.005, 2) -> 0.01
round(0.015, 2) -> 0.02
round(0.025, 2) -> 0.03
round(0.035, 2) -> 0.04
round(0.045, 2) -> 0.05
round(0.055, 2) -> 0.06
round(0.065, 2) -> 0.07
round(0.075, 2) -> 0.08
round(0.085, 2) -> 0.09
