--TEST--
Bug #54254 (cal_days_in_month incompatible with jdtojewish in non-leap-years)
--EXTENSIONS--
calendar
--FILE--
<?php
var_dump(cal_days_in_month(CAL_JEWISH, 1, 5771));
var_dump(cal_days_in_month(CAL_JEWISH, 2, 5771));
var_dump(cal_days_in_month(CAL_JEWISH, 3, 5771));
var_dump(cal_days_in_month(CAL_JEWISH, 4, 5771));
var_dump(cal_days_in_month(CAL_JEWISH, 5, 5771));
var_dump(cal_days_in_month(CAL_JEWISH, 6, 5771));
var_dump(cal_days_in_month(CAL_JEWISH, 7, 5771));
var_dump(cal_days_in_month(CAL_JEWISH, 8, 5771));
var_dump(cal_days_in_month(CAL_JEWISH, 9, 5771));
var_dump(cal_days_in_month(CAL_JEWISH, 10, 5771));
var_dump(cal_days_in_month(CAL_JEWISH, 11, 5771));
var_dump(cal_days_in_month(CAL_JEWISH, 12, 5771));
var_dump(cal_days_in_month(CAL_JEWISH, 13, 5771));
var_dump(cal_days_in_month(CAL_JEWISH, 1, 5772));
var_dump(cal_days_in_month(CAL_JEWISH, 2, 5772));
var_dump(cal_days_in_month(CAL_JEWISH, 3, 5772));
var_dump(cal_days_in_month(CAL_JEWISH, 4, 5772));
var_dump(cal_days_in_month(CAL_JEWISH, 5, 5772));
var_dump(cal_days_in_month(CAL_JEWISH, 6, 5772));
var_dump(cal_days_in_month(CAL_JEWISH, 7, 5772));
var_dump(cal_days_in_month(CAL_JEWISH, 8, 5772));
var_dump(cal_days_in_month(CAL_JEWISH, 9, 5772));
var_dump(cal_days_in_month(CAL_JEWISH, 10, 5772));
var_dump(cal_days_in_month(CAL_JEWISH, 11, 5772));
var_dump(cal_days_in_month(CAL_JEWISH, 12, 5772));
var_dump(cal_days_in_month(CAL_JEWISH, 13, 5772));
?>
--EXPECT--
int(30)
int(30)
int(30)
int(29)
int(30)
int(30)
int(29)
int(30)
int(29)
int(30)
int(29)
int(30)
int(29)
int(30)
int(29)
int(30)
int(29)
int(30)
int(0)
int(29)
int(30)
int(29)
int(30)
int(29)
int(30)
int(29)
