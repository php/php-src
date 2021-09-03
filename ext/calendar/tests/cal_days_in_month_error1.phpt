--TEST--
Test cal_days_in_month() function : error conditions
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--EXTENSIONS--
calendar
--FILE--
<?php
try {
    cal_days_in_month(-1, 4, 2017);
} catch (ValueError $ex) {
    echo "{$ex->getMessage()}\n";
}
try{
    cal_days_in_month(CAL_GREGORIAN,0, 2009);
} catch (ValueError $ex) {
    echo "{$ex->getMessage()}\n";
}
?>
--EXPECT--
cal_days_in_month(): Argument #1 ($calendar) must be a valid calendar ID
Invalid date
