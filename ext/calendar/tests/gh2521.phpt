--TEST--
GH-2521: cal_days_in_month() must not return a negative day count for the last representable year
--EXTENSIONS--
calendar
--FILE--
<?php
// 2147478847 is INT32_MAX - 4800, the largest year GregorianToSdn()/
// JulianToSdn() accept. Asking for month 12 rolls over into year + 1,
// which is out of range: this used to leave sdn_next at 0 and return
// a large negative day count instead of raising.
foreach ([CAL_GREGORIAN, CAL_JULIAN] as $cal) {
    try {
        cal_days_in_month($cal, 12, 2147478847);
        echo "no error (unexpected)\n";
    } catch (\ValueError $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

// The year before, and an earlier month of the boundary year, must still work.
var_dump(cal_days_in_month(CAL_GREGORIAN, 12, 2147478846));
var_dump(cal_days_in_month(CAL_GREGORIAN, 11, 2147478847));

// The French calendar's own end-of-range fallback must be unaffected.
var_dump(cal_days_in_month(CAL_FRENCH, 13, 14));
?>
--EXPECT--
ValueError: Invalid date
ValueError: Invalid date
int(31)
int(30)
int(5)
