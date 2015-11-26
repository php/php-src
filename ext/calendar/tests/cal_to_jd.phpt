--TEST--
cal_to_jd()
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
echo cal_to_jd(CAL_GREGORIAN, 8, 26, 74), "\n";
echo cal_to_jd(CAL_JULIAN, 8, 26, 74), "\n";
echo cal_to_jd(CAL_JEWISH, 8, 26, 74), "\n";
echo cal_to_jd(CAL_FRENCH, 8, 26, 74), "\n";
?>
--EXPECT--
1748326
1748324
374867
0
