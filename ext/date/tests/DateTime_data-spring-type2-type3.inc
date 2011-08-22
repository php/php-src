<?php

/*
 * Note: test names match method names in a set of PHPUnit tests
 * in a userland package.  Please be so kind as to leave them.
 */

date_default_timezone_set('America/New_York');


/*
 * Time, Spring, Zone Type 2 to Zone Type 3
 *
 * + prev: the day before the transition day  2010-03-13 18:38:28 EST
 * + st: standard time on transition day      2010-03-14 00:10:20 EST
 * + dt: daylight time on the transition day  2010-03-14 03:16:55 EDT
 * + post: the day after the transition day   2010-03-15 19:59:59 EDT
 * + stsec: standard time 1 sec before change 2010-03-14 01:59:59 EST
 * + dtsec: daylight time first second        2010-03-14 03:00:00 EDT
 */
echo "test_time_spring_type2_prev_type3_prev: ";
$end   = new DateTime('2010-03-13 18:38:28');  // prev, zt3
$start = new DateTime('2010-02-11 02:18:48 EST');  // sp prev, zt2
examine_diff($end, $start, 'P+0Y1M2DT16H19M40S', 30);

echo "test_time_spring_type2_prev_type3_st: ";
$end   = new DateTime('2010-03-14 00:10:20');  // st, zt3
$start = new DateTime('2010-03-13 18:38:28 EST');  // prev, zt2
examine_diff($end, $start, 'P+0Y0M0DT5H31M52S', 0);

echo "test_time_spring_type2_prev_type3_dt: ";
$end   = new DateTime('2010-03-14 03:16:55');  // dt, zt3
$start = new DateTime('2010-03-13 18:38:28 EST');  // prev, zt2
examine_diff($end, $start, 'P+0Y0M0DT7H38M27S', 0);

echo "test_time_spring_type2_prev_type3_post: ";
$end   = new DateTime('2010-03-15 19:59:59');  // post, zt3
$start = new DateTime('2010-03-13 18:38:28 EST');  // prev, zt2
examine_diff($end, $start, 'P+0Y0M2DT1H21M31S', 2);

echo "test_time_spring_type2_st_type3_prev: ";
$end   = new DateTime('2010-03-13 18:38:28');  // prev, zt3
$start = new DateTime('2010-03-14 00:10:20 EST');  // st, zt2
examine_diff($end, $start, 'P-0Y0M0DT5H31M52S', 0);

echo "test_time_spring_type2_st_type3_st: ";
$end   = new DateTime('2010-03-14 00:15:35');  // sp st, zt3
$start = new DateTime('2010-03-14 00:10:20 EST');  // st, zt2
examine_diff($end, $start, 'P+0Y0M0DT0H5M15S', 0);

echo "test_time_spring_type2_st_type3_dt: ";
$end   = new DateTime('2010-03-14 03:16:55');  // dt, zt3
$start = new DateTime('2010-03-14 00:10:20 EST');  // st, zt2
examine_diff($end, $start, 'P+0Y0M0DT2H6M35S', 0);

echo "test_time_spring_type2_st_type3_post: ";
$end   = new DateTime('2010-03-15 19:59:59');  // post, zt3
$start = new DateTime('2010-03-14 00:10:20 EST');  // st, zt2
examine_diff($end, $start, 'P+0Y0M1DT18H49M39S', 1);

echo "test_time_spring_type2_dt_type3_prev: ";
$end   = new DateTime('2010-03-13 18:38:28');  // prev, zt3
$start = new DateTime('2010-03-14 03:16:55 EDT');  // dt, zt2
examine_diff($end, $start, 'P-0Y0M0DT7H38M27S', 0);

echo "test_time_spring_type2_dt_type3_st: ";
$end   = new DateTime('2010-03-14 00:10:20');  // st, zt3
$start = new DateTime('2010-03-14 03:16:55 EDT');  // dt, zt2
examine_diff($end, $start, 'P-0Y0M0DT2H6M35S', 0);

echo "test_time_spring_type2_dt_type3_dt: ";
$end   = new DateTime('2010-03-14 05:19:56');  // sp dt, zt3
$start = new DateTime('2010-03-14 03:16:55 EDT');  // dt, zt2
examine_diff($end, $start, 'P+0Y0M0DT2H3M1S', 0);

echo "test_time_spring_type2_dt_type3_post: ";
$end   = new DateTime('2010-03-15 19:59:59');  // post, zt3
$start = new DateTime('2010-03-14 03:16:55 EDT');  // dt, zt2
examine_diff($end, $start, 'P+0Y0M1DT16H43M4S', 1);

echo "test_time_spring_type2_post_type3_prev: ";
$end   = new DateTime('2010-03-13 18:38:28');  // prev, zt3
$start = new DateTime('2010-03-15 19:59:59 EDT');  // post, zt2
examine_diff($end, $start, 'P-0Y0M2DT1H21M31S', 2);

echo "test_time_spring_type2_post_type3_st: ";
$end   = new DateTime('2010-03-14 00:10:20');  // st, zt3
$start = new DateTime('2010-03-15 19:59:59 EDT');  // post, zt2
examine_diff($end, $start, 'P-0Y0M1DT18H49M39S', 1);

echo "test_time_spring_type2_post_type3_dt: ";
$end   = new DateTime('2010-03-14 03:16:55');  // dt, zt3
$start = new DateTime('2010-03-15 19:59:59 EDT');  // post, zt2
examine_diff($end, $start, 'P-0Y0M1DT16H43M4S', 1);

echo "test_time_spring_type2_post_type3_post: ";
$end   = new DateTime('2010-03-15 19:59:59');  // post, zt3
$start = new DateTime('2010-03-15 18:57:55 EDT');  // sp post, zt2
examine_diff($end, $start, 'P+0Y0M0DT1H2M4S', 0);

echo "test_time_spring_type2_stsec_type3_dtsec: ";
$end   = new DateTime('2010-03-14 03:00:00');  // dtsec, zt3
$start = new DateTime('2010-03-14 01:59:59 EST');  // stsec, zt2
examine_diff($end, $start, 'P+0Y0M0DT0H0M1S', 0);

echo "test_time_spring_type2_dtsec_type3_stsec: ";
$end   = new DateTime('2010-03-14 01:59:59');  // stsec, zt3
$start = new DateTime('2010-03-14 03:00:00 EDT');  // dtsec, zt2
examine_diff($end, $start, 'P-0Y0M0DT0H0M1S', 0);
