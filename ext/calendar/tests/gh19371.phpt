--TEST--
GH-19371 (integer overflow in calendar.c)
--SKIPIF--
<?php if (PHP_INT_SIZE !== 8) die("skip only for 64-bit"); ?>
--EXTENSIONS--
calendar
--FILE--
<?php

try {
    echo cal_days_in_month(CAL_GREGORIAN, 12, PHP_INT_MAX);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    echo cal_days_in_month(CAL_GREGORIAN, PHP_INT_MIN, 1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    echo cal_days_in_month(CAL_GREGORIAN, PHP_INT_MAX, 1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    echo cal_to_jd(CAL_GREGORIAN, PHP_INT_MIN, 1, 1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    echo cal_to_jd(CAL_GREGORIAN, PHP_INT_MAX, 1, 1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    echo cal_to_jd(CAL_GREGORIAN, 1, PHP_INT_MIN, 1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    echo cal_to_jd(CAL_GREGORIAN, 1, PHP_INT_MAX, 1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    echo cal_to_jd(CAL_GREGORIAN, 1, 1, PHP_INT_MAX);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
cal_days_in_month(): Argument #3 ($year) must be less than 2147483646
cal_days_in_month(): Argument #2 ($month) must be between 1 and 2147483646
cal_days_in_month(): Argument #2 ($month) must be between 1 and 2147483646
cal_to_jd(): Argument #2 ($month) must be between 1 and 2147483646
cal_to_jd(): Argument #2 ($month) must be between 1 and 2147483646
cal_to_jd(): Argument #3 ($day) must be between -2147483648 and 2147483647
cal_to_jd(): Argument #3 ($day) must be between -2147483648 and 2147483647
cal_to_jd(): Argument #4 ($year) must be less than 2147483646
