--TEST--
Bug #24142 (round() problems)
--FILE--
<?php
echo "round(0.005, 2)\n";
var_dump(round(0.005, 2));
echo "\n";

echo "round(0.015, 2)\n";
var_dump(round(0.015, 2));
echo "\n";

echo "round(0.025, 2)\n";
var_dump(round(0.025, 2));
echo "\n";

echo "round(0.035, 2)\n";
var_dump(round(0.035, 2));
echo "\n";

echo "round(0.045, 2)\n";
var_dump(round(0.045, 2));
echo "\n";

echo "round(0.055, 2)\n";
var_dump(round(0.055, 2));
echo "\n";

echo "round(0.065, 2)\n";
var_dump(round(0.065, 2));
echo "\n";

echo "round(0.075, 2)\n";
var_dump(round(0.075, 2));
echo "\n";

echo "round(0.085, 2)\n";
var_dump(round(0.085, 2));
?>
--EXPECT--
round(0.005, 2)
float(0.01)

round(0.015, 2)
float(0.02)

round(0.025, 2)
float(0.03)

round(0.035, 2)
float(0.04)

round(0.045, 2)
float(0.05)

round(0.055, 2)
float(0.06)

round(0.065, 2)
float(0.07)

round(0.075, 2)
float(0.08)

round(0.085, 2)
float(0.09)
