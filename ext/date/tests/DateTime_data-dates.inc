<?php

/*
 * Note: test names match method names in a set of PHPUnit tests
 * in a userland package.  Please be so kind as to leave them.
 */

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

// NOTE: sub() produces different answer.
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

// NOTE: sub() produces different answer.
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
