--TEST--
Bug #25694 (round() and number_format() inconsistency)
--INI--
precision=14
--FILE--
<?php
echo "round 0.045 = " . round(0.045, 2) . "\n";
echo "number format 0.045 = " . number_format(0.045, 2) . "\n\n";
echo "round 0.055 = " . round(0.055, 2) . "\n";
echo "number format 0.055 = " . number_format(0.055, 2) . "\n\n";
echo "round 5.045 = " . round(5.045, 2) . "\n";
echo "number format 5.045 = " . number_format(5.045, 2) . "\n\n";
echo "round 5.055 = " . round(5.055, 2) . "\n";
echo "number format 5.055 = " . number_format(5.055, 2) . "\n\n";
echo "round 3.025 = " . round(3.025, 2) . "\n";
echo "number format 3.025 = " . number_format(3.025, 2) . "\n\n";
echo "round 4.025 = " . round(4.025, 2) . "\n";
echo "number format 4.025 = " . number_format(4.025, 2) . "\n\n";
?>
--EXPECT--
round 0.045 = 0.05
number format 0.045 = 0.05

round 0.055 = 0.06
number format 0.055 = 0.06

round 5.045 = 5.05
number format 5.045 = 5.05

round 5.055 = 5.06
number format 5.055 = 5.06

round 3.025 = 3.03
number format 3.025 = 3.03

round 4.025 = 4.03
number format 4.025 = 4.03
