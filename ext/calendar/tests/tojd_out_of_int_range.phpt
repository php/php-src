--TEST--
*tojd(): arguments outside the int range must not alias into the accepted range
--EXTENSIONS--
calendar
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php
// Values whose low 32 bits land in the accepted range must still be rejected.
$bias = 2 ** 32;

foreach (['gregoriantojd', 'juliantojd', 'jewishtojd', 'frenchtojd'] as $function) {
    echo $function, "\n";
    var_dump($function(1, 1, 1));
    var_dump($function(1 + $bias, 1, 1));
    var_dump($function(1, 1 + $bias, 1));
    try {
        var_dump($function(1, 1, 1 + $bias));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

// cal_to_jd() and cal_days_in_month() reach the same helpers through
// cal_to_jd_func_t, and have no lower bound on $year of their own.
echo "cal_to_jd\n";
var_dump(cal_to_jd(CAL_GREGORIAN, 1, 1, 1 - $bias));
try {
    var_dump(cal_days_in_month(CAL_GREGORIAN, 1, 1 - $bias));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
gregoriantojd
int(1721426)
int(0)
int(0)
int(0)
juliantojd
int(1721424)
int(0)
int(0)
int(0)
jewishtojd
int(347998)
int(0)
int(0)
ValueError: jewishtojd(): Argument #3 ($year) must be between -2147483648 and 2147483647
frenchtojd
int(2375840)
int(0)
int(0)
int(0)
cal_to_jd
int(0)
ValueError: Invalid date
