--TEST--
cal_from_jd()
--EXTENSIONS--
calendar
--FILE--
<?php
print_r(cal_from_jd(1748326, CAL_GREGORIAN));
print_r(cal_from_jd(1748324, CAL_JULIAN));
print_r(cal_from_jd( 374867, CAL_JEWISH));
print_r(cal_from_jd(      0, CAL_FRENCH));
?>
--EXPECT--
Array
(
    [date] => 8/26/74
    [month] => 8
    [day] => 26
    [year] => 74
    [dow] => 0
    [abbrevdayname] => Sun
    [dayname] => Sunday
    [abbrevmonth] => Aug
    [monthname] => August
)
Array
(
    [date] => 8/26/74
    [month] => 8
    [day] => 26
    [year] => 74
    [dow] => 5
    [abbrevdayname] => Fri
    [dayname] => Friday
    [abbrevmonth] => Aug
    [monthname] => August
)
Array
(
    [date] => 8/26/74
    [month] => 8
    [day] => 26
    [year] => 74
    [dow] => 4
    [abbrevdayname] => Thu
    [dayname] => Thursday
    [abbrevmonth] => Nisan
    [monthname] => Nisan
)
Array
(
    [date] => 0/0/0
    [month] => 0
    [day] => 0
    [year] => 0
    [dow] => 1
    [abbrevdayname] => Mon
    [dayname] => Monday
    [abbrevmonth] => 
    [monthname] => 
)
