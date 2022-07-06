<?php

/*
 * Note: test names match method names in a set of PHPUnit tests
 * in a userland package.  Please be so kind as to leave them.
 */

date_default_timezone_set('America/New_York');


/*
 * Time, Fall, Zone Type 2 to Zone Type 2
 *
 * + prev: the day before the transition day  2010-11-06 18:38:28 EDT
 * + dt: daylight time on transition day      2010-11-07 00:10:20 EDT
 * + redodt: daylight time in the redo period 2010-11-07 01:12:33 EDT
 * + redost: standard time in the redo period 2010-11-07 01:14:44 EST
 * + st: standard time on the transition day  2010-11-07 03:16:55 EST
 * + post: the day after the transition day   2010-11-08 19:59:59 EST
 * + dtsec: daylight time 1 sec before change 2010-11-07 01:59:59 EDT
 * + stsec: standard time first second        2010-11-07 01:00:00 EST
 */
echo "test_time_fall_type2_prev_type2_prev: ";
$end   = new DateTime('2010-11-06 18:38:28 EDT');  // prev, zt2
$start = new DateTime('2010-10-04 02:18:48 EDT');  // sp prev, zt2
examine_diff($end, $start, 'P+0Y1M2DT16H19M40S', 33);

echo "test_time_fall_type2_prev_type2_dt: ";
$end   = new DateTime('2010-11-07 00:10:20 EDT');  // dt, zt2
$start = new DateTime('2010-11-06 18:38:28 EDT');  // prev, zt2
examine_diff($end, $start, 'P+0Y0M0DT5H31M52S', 0);

echo "test_time_fall_type2_prev_type2_redodt: ";
$end   = new DateTime('2010-11-07 01:12:33 EDT');  // redodt, zt2
$start = new DateTime('2010-11-06 18:38:28 EDT');  // prev, zt2
examine_diff($end, $start, 'P+0Y0M0DT6H34M5S', 0);

echo "test_time_fall_type2_prev_type2_redost: ";
$end   = new DateTime('2010-11-07 01:14:44 EST');  // redost, zt2
$start = new DateTime('2010-11-06 18:38:28 EDT');  // prev, zt2
examine_diff($end, $start, 'P+0Y0M0DT7H36M16S', 0);

echo "test_time_fall_type2_prev_type2_st: ";
$end   = new DateTime('2010-11-07 03:16:55 EST');  // st, zt2
$start = new DateTime('2010-11-06 18:38:28 EDT');  // prev, zt2
examine_diff($end, $start, 'P+0Y0M0DT9H38M27S', 0);

echo "test_time_fall_type2_prev_type2_post: ";
$end   = new DateTime('2010-11-08 19:59:59 EST');  // post, zt2
$start = new DateTime('2010-11-06 18:38:28 EDT');  // prev, zt2
examine_diff($end, $start, 'P+0Y0M2DT1H21M31S', 2);

echo "test_time_fall_type2_dt_type2_prev: ";
$end   = new DateTime('2010-11-06 18:38:28 EDT');  // prev, zt2
$start = new DateTime('2010-11-07 00:10:20 EDT');  // dt, zt2
examine_diff($end, $start, 'P-0Y0M0DT5H31M52S', 0);

echo "test_time_fall_type2_dt_type2_dt: ";
$end   = new DateTime('2010-11-07 00:15:35 EDT');  // sp dt, zt2
$start = new DateTime('2010-11-07 00:10:20 EDT');  // dt, zt2
examine_diff($end, $start, 'P+0Y0M0DT0H5M15S', 0);

echo "test_time_fall_type2_dt_type2_redodt: ";
$end   = new DateTime('2010-11-07 01:12:33 EDT');  // redodt, zt2
$start = new DateTime('2010-11-07 00:10:20 EDT');  // dt, zt2
examine_diff($end, $start, 'P+0Y0M0DT1H2M13S', 0);

echo "test_time_fall_type2_dt_type2_redost: ";
$end   = new DateTime('2010-11-07 01:14:44 EST');  // redost, zt2
$start = new DateTime('2010-11-07 00:10:20 EDT');  // dt, zt2
examine_diff($end, $start, 'P+0Y0M0DT2H4M24S', 0);

echo "test_time_fall_type2_dt_type2_st: ";
$end   = new DateTime('2010-11-07 03:16:55 EST');  // st, zt2
$start = new DateTime('2010-11-07 00:10:20 EDT');  // dt, zt2
examine_diff($end, $start, 'P+0Y0M0DT4H6M35S', 0);

echo "test_time_fall_type2_dt_type2_post: ";
$end   = new DateTime('2010-11-08 19:59:59 EST');  // post, zt2
$start = new DateTime('2010-11-07 00:10:20 EDT');  // dt, zt2
examine_diff($end, $start, 'P+0Y0M1DT20H49M39S', 1);

echo "test_time_fall_type2_redodt_type2_prev: ";
$end   = new DateTime('2010-11-06 18:38:28 EDT');  // prev, zt2
$start = new DateTime('2010-11-07 01:12:33 EDT');  // redodt, zt2
examine_diff($end, $start, 'P-0Y0M0DT6H34M5S', 0);

echo "test_time_fall_type2_redodt_type2_dt: ";
$end   = new DateTime('2010-11-07 00:10:20 EDT');  // dt, zt2
$start = new DateTime('2010-11-07 01:12:33 EDT');  // redodt, zt2
examine_diff($end, $start, 'P-0Y0M0DT1H2M13S', 0);

echo "test_time_fall_type2_redodt_type2_redodt: ";
$end   = new DateTime('2010-11-07 01:15:35 EDT');  // sp redodt, zt2
$start = new DateTime('2010-11-07 01:12:33 EDT');  // redodt, zt2
examine_diff($end, $start, 'P+0Y0M0DT0H3M2S', 0);

echo "test_time_fall_type2_redodt_type2_redost: ";
$end   = new DateTime('2010-11-07 01:14:44 EST');  // redost, zt2
$start = new DateTime('2010-11-07 01:12:33 EDT');  // redodt, zt2
examine_diff($end, $start, 'P+0Y0M0DT1H2M11S', 0);

echo "test_time_fall_type2_redodt_type2_st: ";
$end   = new DateTime('2010-11-07 03:16:55 EST');  // st, zt2
$start = new DateTime('2010-11-07 01:12:33 EDT');  // redodt, zt2
examine_diff($end, $start, 'P+0Y0M0DT3H4M22S', 0);

echo "test_time_fall_type2_redodt_type2_post: ";
$end   = new DateTime('2010-11-08 19:59:59 EST');  // post, zt2
$start = new DateTime('2010-11-07 01:12:33 EDT');  // redodt, zt2
examine_diff($end, $start, 'P+0Y0M1DT19H47M26S', 1);

echo "test_time_fall_type2_redost_type2_prev: ";
$end   = new DateTime('2010-11-06 18:38:28 EDT');  // prev, zt2
$start = new DateTime('2010-11-07 01:14:44 EST');  // redost, zt2
examine_diff($end, $start, 'P-0Y0M0DT7H36M16S', 0);

echo "test_time_fall_type2_redost_type2_dt: ";
$end   = new DateTime('2010-11-07 00:10:20 EDT');  // dt, zt2
$start = new DateTime('2010-11-07 01:14:44 EST');  // redost, zt2
examine_diff($end, $start, 'P-0Y0M0DT2H4M24S', 0);

echo "test_time_fall_type2_redost_type2_redodt: ";
$end   = new DateTime('2010-11-07 01:12:33 EDT');  // redodt, zt2
$start = new DateTime('2010-11-07 01:14:44 EST');  // redost, zt2
examine_diff($end, $start, 'P-0Y0M0DT1H2M11S', 0);

echo "test_time_fall_type2_redost_type2_redost: ";
$end   = new DateTime('2010-11-07 01:16:54 EST');  // sp redodt, zt2
$start = new DateTime('2010-11-07 01:14:44 EST');  // redost, zt2
examine_diff($end, $start, 'P+0Y0M0DT0H2M10S', 0);

echo "test_time_fall_type2_redost_type2_st: ";
$end   = new DateTime('2010-11-07 03:16:55 EST');  // st, zt2
$start = new DateTime('2010-11-07 01:14:44 EST');  // redost, zt2
examine_diff($end, $start, 'P+0Y0M0DT2H2M11S', 0);

echo "test_time_fall_type2_redost_type2_post: ";
$end   = new DateTime('2010-11-08 19:59:59 EST');  // post, zt2
$start = new DateTime('2010-11-07 01:14:44 EST');  // redost, zt2
examine_diff($end, $start, 'P+0Y0M1DT18H45M15S', 1);

echo "test_time_fall_type2_st_type2_prev: ";
$end   = new DateTime('2010-11-06 18:38:28 EDT');  // prev, zt2
$start = new DateTime('2010-11-07 03:16:55 EST');  // st, zt2
examine_diff($end, $start, 'P-0Y0M0DT9H38M27S', 0);

echo "test_time_fall_type2_st_type2_dt: ";
$end   = new DateTime('2010-11-07 00:10:20 EDT');  // dt, zt2
$start = new DateTime('2010-11-07 03:16:55 EST');  // st, zt2
examine_diff($end, $start, 'P-0Y0M0DT4H6M35S', 0);

echo "test_time_fall_type2_st_type2_redodt: ";
$end   = new DateTime('2010-11-07 01:12:33 EDT');  // redodt, zt2
$start = new DateTime('2010-11-07 03:16:55 EST');  // st, zt2
examine_diff($end, $start, 'P-0Y0M0DT3H4M22S', 0);

echo "test_time_fall_type2_st_type2_redost: ";
$end   = new DateTime('2010-11-07 01:14:44 EST');  // redost, zt2
$start = new DateTime('2010-11-07 03:16:55 EST');  // st, zt2
examine_diff($end, $start, 'P-0Y0M0DT2H2M11S', 0);

echo "test_time_fall_type2_st_type2_st: ";
$end   = new DateTime('2010-11-07 05:19:56 EST');  // sp st, zt2
$start = new DateTime('2010-11-07 03:16:55 EST');  // st, zt2
examine_diff($end, $start, 'P+0Y0M0DT2H3M1S', 0);

echo "test_time_fall_type2_st_type2_post: ";
$end   = new DateTime('2010-11-08 19:59:59 EST');  // post, zt2
$start = new DateTime('2010-11-07 03:16:55 EST');  // st, zt2
examine_diff($end, $start, 'P+0Y0M1DT16H43M4S', 1);

echo "test_time_fall_type2_post_type2_prev: ";
$end   = new DateTime('2010-11-06 18:38:28 EDT');  // prev, zt2
$start = new DateTime('2010-11-08 19:59:59 EST');  // post, zt2
examine_diff($end, $start, 'P-0Y0M2DT1H21M31S', 2);

echo "test_time_fall_type2_post_type2_dt: ";
$end   = new DateTime('2010-11-07 00:10:20 EDT');  // dt, zt2
$start = new DateTime('2010-11-08 19:59:59 EST');  // post, zt2
examine_diff($end, $start, 'P-0Y0M1DT20H49M39S', 1);

echo "test_time_fall_type2_post_type2_redodt: ";
$end   = new DateTime('2010-11-07 01:12:33 EDT');  // redodt, zt2
$start = new DateTime('2010-11-08 19:59:59 EST');  // post, zt2
examine_diff($end, $start, 'P-0Y0M1DT19H47M26S', 1);

echo "test_time_fall_type2_post_type2_redost: ";
$end   = new DateTime('2010-11-07 01:14:44 EST');  // redost, zt2
$start = new DateTime('2010-11-08 19:59:59 EST');  // post, zt2
examine_diff($end, $start, 'P-0Y0M1DT18H45M15S', 1);

echo "test_time_fall_type2_post_type2_st: ";
$end   = new DateTime('2010-11-07 03:16:55 EST');  // st, zt2
$start = new DateTime('2010-11-08 19:59:59 EST');  // post, zt2
examine_diff($end, $start, 'P-0Y0M1DT16H43M4S', 1);

echo "test_time_fall_type2_post_type2_post: ";
$end   = new DateTime('2010-11-08 19:59:59 EST');  // post, zt2
$start = new DateTime('2010-11-08 18:57:55 EST');  // sp post, zt2
examine_diff($end, $start, 'P+0Y0M0DT1H2M4S', 0);

echo "test_time_fall_type2_dtsec_type2_stsec: ";
$end   = new DateTime('2010-11-07 01:00:00 EST');  // stsec, zt2
$start = new DateTime('2010-11-07 01:59:59 EDT');  // dtsec, zt2
examine_diff($end, $start, 'P+0Y0M0DT0H0M1S', 0);

echo "test_time_fall_type2_stsec_type2_dtsec: ";
$end   = new DateTime('2010-11-07 01:59:59 EDT');  // dtsec, zt2
$start = new DateTime('2010-11-07 01:00:00 EST');  // stsec, zt2
examine_diff($end, $start, 'P-0Y0M0DT0H0M1S', 0);
