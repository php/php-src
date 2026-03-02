--TEST--
Test cal_to_jd() function : error conditions
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--EXTENSIONS--
calendar
--FILE--
<?php
try {
    cal_to_jd(-1, 8, 26, 74);
} catch (ValueError $ex) {
    echo "{$ex->getMessage()}\n";
}
?>
--EXPECT--
cal_to_jd(): Argument #1 ($calendar) must be a valid calendar ID
