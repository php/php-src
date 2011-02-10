--TEST--
DateTime::diff() add() sub() -- spring type3 type2
--CREDITS--
Daniel Convissor <danielc@php.net>
--XFAIL--
PHP < 5.4 has bugs
--FILE--
<?php

/*
 * Note: test names match method names in a set of PHPUnit tests
 * in a userland package.  Please be so kind as to leave them.
 */

require './examine_diff.inc';
date_default_timezone_set('America/New_York');


/*
 * Time, Spring, Zone Type 3 to Zone Type 2
 *
 * + prev: the day before the transition day  2010-03-13 18:38:28 EST
 * + st: standard time on transition day      2010-03-14 00:10:20 EST
 * + dt: daylight time on the transition day  2010-03-14 03:16:55 EDT
 * + post: the day after the transition day   2010-03-15 19:59:59 EDT
 */
echo "test_time_spring_type3_prev_type2_prev: ";
$end   = new DateTime('2010-03-13 18:38:28 EST');  // prev, zt2
$start = new DateTime('2010-02-11 02:18:48');  // sp prev, zt3
examine_diff($end, $start, 'P+0Y1M2DT16H19M40S', 30);

echo "test_time_spring_type3_prev_type2_st: ";
$end   = new DateTime('2010-03-14 00:10:20 EST');  // st, zt2
$start = new DateTime('2010-03-13 18:38:28');  // prev, zt3
examine_diff($end, $start, 'P+0Y0M0DT5H31M52S', 0);

echo "test_time_spring_type3_prev_type2_dt: ";
$end   = new DateTime('2010-03-14 03:16:55 EDT');  // dt, zt2
$start = new DateTime('2010-03-13 18:38:28');  // prev, zt3
examine_diff($end, $start, 'P+0Y0M0DT7H38M27S', 0);

echo "test_time_spring_type3_prev_type2_post: ";
$end   = new DateTime('2010-03-15 19:59:59 EDT');  // post, zt2
$start = new DateTime('2010-03-13 18:38:28');  // prev, zt3
examine_diff($end, $start, 'P+0Y0M2DT1H21M31S', 2);

echo "test_time_spring_type3_st_type2_prev: ";
$end   = new DateTime('2010-03-13 18:38:28 EST');  // prev, zt2
$start = new DateTime('2010-03-14 00:10:20');  // st, zt3
examine_diff($end, $start, 'P-0Y0M0DT5H31M52S', 0);

echo "test_time_spring_type3_st_type2_st: ";
$end   = new DateTime('2010-03-14 00:15:35 EST');  // sp st, zt2
$start = new DateTime('2010-03-14 00:10:20');  // st, zt3
examine_diff($end, $start, 'P+0Y0M0DT0H5M15S', 0);

echo "test_time_spring_type3_st_type2_dt: ";
$end   = new DateTime('2010-03-14 03:16:55 EDT');  // dt, zt2
$start = new DateTime('2010-03-14 00:10:20');  // st, zt3
examine_diff($end, $start, 'P+0Y0M0DT2H6M35S', 0);

echo "test_time_spring_type3_st_type2_post: ";
$end   = new DateTime('2010-03-15 19:59:59 EDT');  // post, zt2
$start = new DateTime('2010-03-14 00:10:20');  // st, zt3
examine_diff($end, $start, 'P+0Y0M1DT18H49M39S', 1);

echo "test_time_spring_type3_dt_type2_prev: ";
$end   = new DateTime('2010-03-13 18:38:28 EST');  // prev, zt2
$start = new DateTime('2010-03-14 03:16:55');  // dt, zt3
examine_diff($end, $start, 'P-0Y0M0DT7H38M27S', 0);

echo "test_time_spring_type3_dt_type2_st: ";
$end   = new DateTime('2010-03-14 00:10:20 EST');  // st, zt2
$start = new DateTime('2010-03-14 03:16:55');  // dt, zt3
examine_diff($end, $start, 'P-0Y0M0DT2H6M35S', 0);

echo "test_time_spring_type3_dt_type2_dt: ";
$end   = new DateTime('2010-03-14 05:19:56 EDT');  // sp dt, zt2
$start = new DateTime('2010-03-14 03:16:55');  // dt, zt3
examine_diff($end, $start, 'P+0Y0M0DT2H3M1S', 0);

echo "test_time_spring_type3_dt_type2_post: ";
$end   = new DateTime('2010-03-15 19:59:59 EDT');  // post, zt2
$start = new DateTime('2010-03-14 03:16:55');  // dt, zt3
examine_diff($end, $start, 'P+0Y0M1DT16H43M4S', 1);

echo "test_time_spring_type3_post_type2_prev: ";
$end   = new DateTime('2010-03-13 18:38:28 EST');  // prev, zt2
$start = new DateTime('2010-03-15 19:59:59');  // post, zt3
examine_diff($end, $start, 'P-0Y0M2DT1H21M31S', 2);

echo "test_time_spring_type3_post_type2_st: ";
$end   = new DateTime('2010-03-14 00:10:20 EST');  // st, zt2
$start = new DateTime('2010-03-15 19:59:59');  // post, zt3
examine_diff($end, $start, 'P-0Y0M1DT18H49M39S', 1);

echo "test_time_spring_type3_post_type2_dt: ";
$end   = new DateTime('2010-03-14 03:16:55 EDT');  // dt, zt2
$start = new DateTime('2010-03-15 19:59:59');  // post, zt3
examine_diff($end, $start, 'P-0Y0M1DT16H43M4S', 1);

echo "test_time_spring_type3_post_type2_post: ";
$end   = new DateTime('2010-03-15 19:59:59 EDT');  // post, zt2
$start = new DateTime('2010-03-15 18:57:55');  // sp post, zt3
examine_diff($end, $start, 'P+0Y0M0DT1H2M4S', 0);

?>
--EXPECT--
test_time_spring_type3_prev_type2_prev: FWD: 2010-03-13 18:38:28 EST - 2010-02-11 02:18:48 EST = **P+0Y1M2DT16H19M40S** | BACK: 2010-02-11 02:18:48 EST + P+0Y1M2DT16H19M40S = **2010-03-13 18:38:28 EST** | DAYS: **30**
test_time_spring_type3_prev_type2_st: FWD: 2010-03-14 00:10:20 EST - 2010-03-13 18:38:28 EST = **P+0Y0M0DT5H31M52S** | BACK: 2010-03-13 18:38:28 EST + P+0Y0M0DT5H31M52S = **2010-03-14 00:10:20 EST** | DAYS: **0**
test_time_spring_type3_prev_type2_dt: FWD: 2010-03-14 03:16:55 EDT - 2010-03-13 18:38:28 EST = **P+0Y0M0DT7H38M27S** | BACK: 2010-03-13 18:38:28 EST + P+0Y0M0DT7H38M27S = **2010-03-14 03:16:55 EDT** | DAYS: **0**
test_time_spring_type3_prev_type2_post: FWD: 2010-03-15 19:59:59 EDT - 2010-03-13 18:38:28 EST = **P+0Y0M2DT1H21M31S** | BACK: 2010-03-13 18:38:28 EST + P+0Y0M2DT1H21M31S = **2010-03-15 19:59:59 EDT** | DAYS: **2**
test_time_spring_type3_st_type2_prev: FWD: 2010-03-13 18:38:28 EST - 2010-03-14 00:10:20 EST = **P-0Y0M0DT5H31M52S** | BACK: 2010-03-14 00:10:20 EST + P-0Y0M0DT5H31M52S = **2010-03-13 18:38:28 EST** | DAYS: **0**
test_time_spring_type3_st_type2_st: FWD: 2010-03-14 00:15:35 EST - 2010-03-14 00:10:20 EST = **P+0Y0M0DT0H5M15S** | BACK: 2010-03-14 00:10:20 EST + P+0Y0M0DT0H5M15S = **2010-03-14 00:15:35 EST** | DAYS: **0**
test_time_spring_type3_st_type2_dt: FWD: 2010-03-14 03:16:55 EDT - 2010-03-14 00:10:20 EST = **P+0Y0M0DT2H6M35S** | BACK: 2010-03-14 00:10:20 EST + P+0Y0M0DT2H6M35S = **2010-03-14 03:16:55 EDT** | DAYS: **0**
test_time_spring_type3_st_type2_post: FWD: 2010-03-15 19:59:59 EDT - 2010-03-14 00:10:20 EST = **P+0Y0M1DT18H49M39S** | BACK: 2010-03-14 00:10:20 EST + P+0Y0M1DT18H49M39S = **2010-03-15 19:59:59 EDT** | DAYS: **1**
test_time_spring_type3_dt_type2_prev: FWD: 2010-03-13 18:38:28 EST - 2010-03-14 03:16:55 EDT = **P-0Y0M0DT7H38M27S** | BACK: 2010-03-14 03:16:55 EDT + P-0Y0M0DT7H38M27S = **2010-03-13 18:38:28 EST** | DAYS: **0**
test_time_spring_type3_dt_type2_st: FWD: 2010-03-14 00:10:20 EST - 2010-03-14 03:16:55 EDT = **P-0Y0M0DT2H6M35S** | BACK: 2010-03-14 03:16:55 EDT + P-0Y0M0DT2H6M35S = **2010-03-14 00:10:20 EST** | DAYS: **0**
test_time_spring_type3_dt_type2_dt: FWD: 2010-03-14 05:19:56 EDT - 2010-03-14 03:16:55 EDT = **P+0Y0M0DT2H3M1S** | BACK: 2010-03-14 03:16:55 EDT + P+0Y0M0DT2H3M1S = **2010-03-14 05:19:56 EDT** | DAYS: **0**
test_time_spring_type3_dt_type2_post: FWD: 2010-03-15 19:59:59 EDT - 2010-03-14 03:16:55 EDT = **P+0Y0M1DT16H43M4S** | BACK: 2010-03-14 03:16:55 EDT + P+0Y0M1DT16H43M4S = **2010-03-15 19:59:59 EDT** | DAYS: **1**
test_time_spring_type3_post_type2_prev: FWD: 2010-03-13 18:38:28 EST - 2010-03-15 19:59:59 EDT = **P-0Y0M2DT1H21M31S** | BACK: 2010-03-15 19:59:59 EDT + P-0Y0M2DT1H21M31S = **2010-03-13 18:38:28 EST** | DAYS: **2**
test_time_spring_type3_post_type2_st: FWD: 2010-03-14 00:10:20 EST - 2010-03-15 19:59:59 EDT = **P-0Y0M1DT18H49M39S** | BACK: 2010-03-15 19:59:59 EDT + P-0Y0M1DT18H49M39S = **2010-03-14 00:10:20 EST** | DAYS: **1**
test_time_spring_type3_post_type2_dt: FWD: 2010-03-14 03:16:55 EDT - 2010-03-15 19:59:59 EDT = **P-0Y0M1DT16H43M4S** | BACK: 2010-03-15 19:59:59 EDT + P-0Y0M1DT16H43M4S = **2010-03-14 03:16:55 EDT** | DAYS: **1**
test_time_spring_type3_post_type2_post: FWD: 2010-03-15 19:59:59 EDT - 2010-03-15 18:57:55 EDT = **P+0Y0M0DT1H2M4S** | BACK: 2010-03-15 18:57:55 EDT + P+0Y0M0DT1H2M4S = **2010-03-15 19:59:59 EDT** | DAYS: **0**
