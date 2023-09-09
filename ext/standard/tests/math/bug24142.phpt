--TEST--
Bug #24142 (round() problems)
--FILE--
<?php
echo 'round(0.005, 2) -> '.round(0.005, 2)."\n";
echo 'round(0.015, 2) -> '.round(0.015, 2)."\n";
echo 'round(0.025, 2) -> '.round(0.025, 2)."\n";
echo 'round(0.035, 2) -> '.round(0.035, 2)."\n";
echo 'round(0.045, 2) -> '.round(0.045, 2)."\n";
echo 'round(0.055, 2) -> '.round(0.055, 2)."\n";
echo 'round(0.065, 2) -> '.round(0.065, 2)."\n";
echo 'round(0.075, 2) -> '.round(0.075, 2)."\n";
echo 'round(0.085, 2) -> '.round(0.085, 2)."\n";
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
