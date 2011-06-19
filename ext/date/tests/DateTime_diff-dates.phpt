--TEST--
DateTime::diff() -- dates 
--CREDITS--
Daniel Convissor <danielc@php.net>
--FILE--
<?php

require 'examine_diff.inc';
define('PHPT_DATETIME_SHOW', PHPT_DATETIME_SHOW_DIFF);
require 'DateTime_data-dates.inc';

?>
--EXPECT--
test__7: DIFF: 2009-01-14 00:00:00 EST - 2009-01-07 00:00:00 EST = **P+0Y0M7DT0H0M0S**
test_years_positive__7_by_0_day: DIFF: 2007-02-07 00:00:00 EST - 2000-02-07 00:00:00 EST = **P+7Y0M0DT0H0M0S**
test_years_positive__7_by_1_day: DIFF: 2007-02-08 00:00:00 EST - 2000-02-07 00:00:00 EST = **P+7Y0M1DT0H0M0S**
test_years_positive__6_shy_1_day: DIFF: 2007-02-06 00:00:00 EST - 2000-02-07 00:00:00 EST = **P+6Y11M30DT0H0M0S**
test_years_positive__7_by_1_month: DIFF: 2007-03-07 00:00:00 EST - 2000-02-07 00:00:00 EST = **P+7Y1M0DT0H0M0S**
test_years_positive__6_shy_1_month: DIFF: 2007-01-07 00:00:00 EST - 2000-02-07 00:00:00 EST = **P+6Y11M0DT0H0M0S**
test_years_positive__7_by_1_month_split_newyear: DIFF: 2007-01-07 00:00:00 EST - 1999-12-07 00:00:00 EST = **P+7Y1M0DT0H0M0S**
test_years_positive__6_shy_1_month_split_newyear: DIFF: 2006-12-07 00:00:00 EST - 2000-01-07 00:00:00 EST = **P+6Y11M0DT0H0M0S**
test_negative__7: DIFF: 2009-01-07 00:00:00 EST - 2009-01-14 00:00:00 EST = **P-0Y0M7DT0H0M0S**
test_years_negative__7_by_0_day: DIFF: 2000-02-07 00:00:00 EST - 2007-02-07 00:00:00 EST = **P-7Y0M0DT0H0M0S**
test_years_negative__7_by_1_day: DIFF: 2000-02-07 00:00:00 EST - 2007-02-08 00:00:00 EST = **P-7Y0M1DT0H0M0S**
test_years_negative__6_shy_1_day: DIFF: 2000-02-07 00:00:00 EST - 2007-02-06 00:00:00 EST = **P-6Y11M28DT0H0M0S**
test_years_negative__7_by_1_month: DIFF: 2000-02-07 00:00:00 EST - 2007-03-07 00:00:00 EST = **P-7Y1M0DT0H0M0S**
test_years_negative__6_shy_1_month: DIFF: 2000-02-07 00:00:00 EST - 2007-01-07 00:00:00 EST = **P-6Y11M0DT0H0M0S**
test_years_negative__7_by_1_month_split_newyear: DIFF: 1999-12-07 00:00:00 EST - 2007-01-07 00:00:00 EST = **P-7Y1M0DT0H0M0S**
test_years_negative__6_shy_1_month_split_newyear: DIFF: 2000-01-07 00:00:00 EST - 2006-12-07 00:00:00 EST = **P-6Y11M0DT0H0M0S**
