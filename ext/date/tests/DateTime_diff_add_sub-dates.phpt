--TEST--
DateTime::diff() add() sub() -- dates 
--CREDITS--
Daniel Convissor <danielc@php.net>
--FILE--
<?php

/*
 * Note: test names match method names in a set of PHPUnit tests
 * in a userland package.  Please be so kind as to leave them.
 */

require './examine_diff.inc';
date_default_timezone_set('America/New_York');


/*
 * Particular days
 */
echo "test__7: ";
examine_diff('2009-01-14', '2009-01-07', 'P+0Y0M7DT0H0M0S', 7);

echo "test_years_positive__7_by_0_day: ";
examine_diff('2007-02-07', '2000-02-07', 'P+7Y0M0DT0H0M0S', 2557);

echo "test_years_positive__7_by_1_day: ";
examine_diff('2007-02-08', '2000-02-07', 'P+7Y0M1DT0H0M0S', 2558);

echo "test_years_positive__6_shy_1_day: ";
examine_diff('2007-02-06', '2000-02-07', 'P+6Y11M30DT0H0M0S', 2556);

echo "test_years_positive__7_by_1_month: ";
examine_diff('2007-03-07', '2000-02-07', 'P+7Y1M0DT0H0M0S', 2585);

echo "test_years_positive__6_shy_1_month: ";
examine_diff('2007-01-07', '2000-02-07', 'P+6Y11M0DT0H0M0S', 2526);

echo "test_years_positive__7_by_1_month_split_newyear: ";
examine_diff('2007-01-07', '1999-12-07', 'P+7Y1M0DT0H0M0S', 2588);

echo "test_years_positive__6_shy_1_month_split_newyear: ";
examine_diff('2006-12-07', '2000-01-07', 'P+6Y11M0DT0H0M0S', 2526);


/*
 * Particular days, negative
 */
echo "test_negative__7: ";
examine_diff('2009-01-07', '2009-01-14', 'P-0Y0M7DT0H0M0S', 7);

echo "test_years_negative__7_by_0_day: ";
examine_diff('2000-02-07', '2007-02-07', 'P-7Y0M0DT0H0M0S', 2557);

echo "test_years_negative__7_by_1_day: ";
examine_diff('2000-02-07', '2007-02-08', 'P-7Y0M1DT0H0M0S', 2558);

echo "test_years_negative__6_shy_1_day: ";
examine_diff('2000-02-07', '2007-02-06', 'P-6Y11M28DT0H0M0S', 2556);

echo "test_years_negative__7_by_1_month: ";
examine_diff('2000-02-07', '2007-03-07', 'P-7Y1M0DT0H0M0S', 2585);

echo "test_years_negative__6_shy_1_month: ";
examine_diff('2000-02-07', '2007-01-07', 'P-6Y11M0DT0H0M0S', 2526);

echo "test_years_negative__7_by_1_month_split_newyear: ";
examine_diff('1999-12-07', '2007-01-07', 'P-7Y1M0DT0H0M0S', 2588);

echo "test_years_negative__6_shy_1_month_split_newyear: ";
examine_diff('2000-01-07', '2006-12-07', 'P-6Y11M0DT0H0M0S', 2526);

?>
--EXPECT--
test__7: FWD: 2009-01-14 00:00:00 EST - 2009-01-07 00:00:00 EST = **P+0Y0M7DT0H0M0S** | BACK: 2009-01-07 00:00:00 EST + P+0Y0M7DT0H0M0S = **2009-01-14 00:00:00 EST** | DAYS: **7**
test_years_positive__7_by_0_day: FWD: 2007-02-07 00:00:00 EST - 2000-02-07 00:00:00 EST = **P+7Y0M0DT0H0M0S** | BACK: 2000-02-07 00:00:00 EST + P+7Y0M0DT0H0M0S = **2007-02-07 00:00:00 EST** | DAYS: **2557**
test_years_positive__7_by_1_day: FWD: 2007-02-08 00:00:00 EST - 2000-02-07 00:00:00 EST = **P+7Y0M1DT0H0M0S** | BACK: 2000-02-07 00:00:00 EST + P+7Y0M1DT0H0M0S = **2007-02-08 00:00:00 EST** | DAYS: **2558**
test_years_positive__6_shy_1_day: FWD: 2007-02-06 00:00:00 EST - 2000-02-07 00:00:00 EST = **P+6Y11M30DT0H0M0S** | BACK: 2000-02-07 00:00:00 EST + P+6Y11M30DT0H0M0S = **2007-02-06 00:00:00 EST** | DAYS: **2556**
test_years_positive__7_by_1_month: FWD: 2007-03-07 00:00:00 EST - 2000-02-07 00:00:00 EST = **P+7Y1M0DT0H0M0S** | BACK: 2000-02-07 00:00:00 EST + P+7Y1M0DT0H0M0S = **2007-03-07 00:00:00 EST** | DAYS: **2585**
test_years_positive__6_shy_1_month: FWD: 2007-01-07 00:00:00 EST - 2000-02-07 00:00:00 EST = **P+6Y11M0DT0H0M0S** | BACK: 2000-02-07 00:00:00 EST + P+6Y11M0DT0H0M0S = **2007-01-07 00:00:00 EST** | DAYS: **2526**
test_years_positive__7_by_1_month_split_newyear: FWD: 2007-01-07 00:00:00 EST - 1999-12-07 00:00:00 EST = **P+7Y1M0DT0H0M0S** | BACK: 1999-12-07 00:00:00 EST + P+7Y1M0DT0H0M0S = **2007-01-07 00:00:00 EST** | DAYS: **2588**
test_years_positive__6_shy_1_month_split_newyear: FWD: 2006-12-07 00:00:00 EST - 2000-01-07 00:00:00 EST = **P+6Y11M0DT0H0M0S** | BACK: 2000-01-07 00:00:00 EST + P+6Y11M0DT0H0M0S = **2006-12-07 00:00:00 EST** | DAYS: **2526**
test_negative__7: FWD: 2009-01-07 00:00:00 EST - 2009-01-14 00:00:00 EST = **P-0Y0M7DT0H0M0S** | BACK: 2009-01-14 00:00:00 EST + P-0Y0M7DT0H0M0S = **2009-01-07 00:00:00 EST** | DAYS: **7**
test_years_negative__7_by_0_day: FWD: 2000-02-07 00:00:00 EST - 2007-02-07 00:00:00 EST = **P-7Y0M0DT0H0M0S** | BACK: 2007-02-07 00:00:00 EST + P-7Y0M0DT0H0M0S = **2000-02-07 00:00:00 EST** | DAYS: **2557**
test_years_negative__7_by_1_day: FWD: 2000-02-07 00:00:00 EST - 2007-02-08 00:00:00 EST = **P-7Y0M1DT0H0M0S** | BACK: 2007-02-08 00:00:00 EST + P-7Y0M1DT0H0M0S = **2000-02-07 00:00:00 EST** | DAYS: **2558**
test_years_negative__6_shy_1_day: FWD: 2000-02-07 00:00:00 EST - 2007-02-06 00:00:00 EST = **P-6Y11M28DT0H0M0S** | BACK: 2007-02-06 00:00:00 EST + P-6Y11M28DT0H0M0S = **2000-02-07 00:00:00 EST** | DAYS: **2556**
test_years_negative__7_by_1_month: FWD: 2000-02-07 00:00:00 EST - 2007-03-07 00:00:00 EST = **P-7Y1M0DT0H0M0S** | BACK: 2007-03-07 00:00:00 EST + P-7Y1M0DT0H0M0S = **2000-02-07 00:00:00 EST** | DAYS: **2585**
test_years_negative__6_shy_1_month: FWD: 2000-02-07 00:00:00 EST - 2007-01-07 00:00:00 EST = **P-6Y11M0DT0H0M0S** | BACK: 2007-01-07 00:00:00 EST + P-6Y11M0DT0H0M0S = **2000-02-07 00:00:00 EST** | DAYS: **2526**
test_years_negative__7_by_1_month_split_newyear: FWD: 1999-12-07 00:00:00 EST - 2007-01-07 00:00:00 EST = **P-7Y1M0DT0H0M0S** | BACK: 2007-01-07 00:00:00 EST + P-7Y1M0DT0H0M0S = **1999-12-07 00:00:00 EST** | DAYS: **2588**
test_years_negative__6_shy_1_month_split_newyear: FWD: 2000-01-07 00:00:00 EST - 2006-12-07 00:00:00 EST = **P-6Y11M0DT0H0M0S** | BACK: 2006-12-07 00:00:00 EST + P-6Y11M0DT0H0M0S = **2000-01-07 00:00:00 EST** | DAYS: **2526**
