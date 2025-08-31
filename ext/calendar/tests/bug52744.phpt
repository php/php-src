--TEST--
Bug #52744 (cal_days_in_month incorrect for December 1 BCE)
--EXTENSIONS--
calendar
--FILE--
<?php
var_dump(cal_days_in_month(CAL_GREGORIAN, 12, -1));
var_dump(cal_days_in_month(CAL_JULIAN, 12, -1));
?>
--EXPECT--
int(31)
int(31)
