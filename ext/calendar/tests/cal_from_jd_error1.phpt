--TEST--
Test cal_from_jd() function : error conditions
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--EXTENSIONS--
calendar
--FILE--
<?php
try {
    cal_from_jd(1748326, -1);
} catch (ValueError $ex) {
    echo "{$ex->getMessage()}\n";
}
?>
--EXPECT--
cal_from_jd(): Argument #2 ($calendar) must be a valid calendar ID
