--TEST--
DateTime::diff() days -- dates 
--CREDITS--
Daniel Convissor <danielc@php.net>
--FILE--
<?php

require 'examine_diff.inc';
define('PHPT_DATETIME_SHOW', PHPT_DATETIME_SHOW_DAYS);
require 'DateTime_data-dates.inc';

?>
--EXPECT--
test__7: DAYS: **7**
test_years_positive__7_by_0_day: DAYS: **2557**
test_years_positive__7_by_1_day: DAYS: **2558**
test_years_positive__6_shy_1_day: DAYS: **2556**
test_years_positive__7_by_1_month: DAYS: **2585**
test_years_positive__6_shy_1_month: DAYS: **2526**
test_years_positive__7_by_1_month_split_newyear: DAYS: **2588**
test_years_positive__6_shy_1_month_split_newyear: DAYS: **2526**
test_negative__7: DAYS: **7**
test_years_negative__7_by_0_day: DAYS: **2557**
test_years_negative__7_by_1_day: DAYS: **2558**
test_years_negative__6_shy_1_day: DAYS: **2556**
test_years_negative__7_by_1_month: DAYS: **2585**
test_years_negative__6_shy_1_month: DAYS: **2526**
test_years_negative__7_by_1_month_split_newyear: DAYS: **2588**
test_years_negative__6_shy_1_month_split_newyear: DAYS: **2526**
