--TEST--
DateTime::diff() -- fall type3 type3
--CREDITS--
Daniel Convissor <danielc@php.net>
--FILE--
<?php

require 'examine_diff.inc';
define('PHPT_DATETIME_SHOW', PHPT_DATETIME_SHOW_DIFF);
require 'DateTime_data-fall-type3-type3.inc';

?>
--EXPECT--
test_time_fall_type3_prev_type3_prev: DIFF: 2010-11-06 18:38:28 EDT - 2010-10-04 02:18:48 EDT = **P+0Y1M2DT16H19M40S**
test_time_fall_type3_prev_type3_dt: DIFF: 2010-11-07 00:10:20 EDT - 2010-11-06 18:38:28 EDT = **P+0Y0M0DT5H31M52S**
test_time_fall_type3_prev_type3_redodt: DIFF: 2010-11-07 01:12:33 EDT - 2010-11-06 18:38:28 EDT = **P+0Y0M0DT6H34M5S**
test_time_fall_type3_prev_type3_redost: DIFF: 2010-11-07 01:14:44 EST - 2010-11-06 18:38:28 EDT = **P+0Y0M0DT7H36M16S**
test_time_fall_type3_prev_type3_st: DIFF: 2010-11-07 03:16:55 EST - 2010-11-06 18:38:28 EDT = **P+0Y0M0DT9H38M27S**
test_time_fall_type3_prev_type3_post: DIFF: 2010-11-08 19:59:59 EST - 2010-11-06 18:38:28 EDT = **P+0Y0M2DT1H21M31S**
test_time_fall_type3_dt_type3_prev: DIFF: 2010-11-06 18:38:28 EDT - 2010-11-07 00:10:20 EDT = **P-0Y0M0DT5H31M52S**
test_time_fall_type3_dt_type3_dt: DIFF: 2010-11-07 00:15:35 EDT - 2010-11-07 00:10:20 EDT = **P+0Y0M0DT0H5M15S**
test_time_fall_type3_dt_type3_redodt: DIFF: 2010-11-07 01:12:33 EDT - 2010-11-07 00:10:20 EDT = **P+0Y0M0DT1H2M13S**
test_time_fall_type3_dt_type3_redost: DIFF: 2010-11-07 01:14:44 EST - 2010-11-07 00:10:20 EDT = **P+0Y0M0DT2H4M24S**
test_time_fall_type3_dt_type3_st: DIFF: 2010-11-07 03:16:55 EST - 2010-11-07 00:10:20 EDT = **P+0Y0M0DT4H6M35S**
test_time_fall_type3_dt_type3_post: DIFF: 2010-11-08 19:59:59 EST - 2010-11-07 00:10:20 EDT = **P+0Y0M1DT19H49M39S**
test_time_fall_type3_redodt_type3_prev: DIFF: 2010-11-06 18:38:28 EDT - 2010-11-07 01:12:33 EDT = **P-0Y0M0DT6H34M5S**
test_time_fall_type3_redodt_type3_dt: DIFF: 2010-11-07 00:10:20 EDT - 2010-11-07 01:12:33 EDT = **P-0Y0M0DT1H2M13S**
test_time_fall_type3_redodt_type3_redodt: DIFF: 2010-11-07 01:15:35 EDT - 2010-11-07 01:12:33 EDT = **P+0Y0M0DT0H3M2S**
test_time_fall_type3_redodt_type3_redost: DIFF: 2010-11-07 01:14:44 EST - 2010-11-07 01:12:33 EDT = **P+0Y0M0DT1H2M11S**
test_time_fall_type3_redodt_type3_st: DIFF: 2010-11-07 03:16:55 EST - 2010-11-07 01:12:33 EDT = **P+0Y0M0DT3H4M22S**
test_time_fall_type3_redodt_type3_post: DIFF: 2010-11-08 19:59:59 EST - 2010-11-07 01:12:33 EDT = **P+0Y0M1DT18H47M26S**
test_time_fall_type3_redost_type3_prev: DIFF: 2010-11-06 18:38:28 EDT - 2010-11-07 01:14:44 EST = **P-0Y0M0DT7H36M16S**
test_time_fall_type3_redost_type3_dt: DIFF: 2010-11-07 00:10:20 EDT - 2010-11-07 01:14:44 EST = **P-0Y0M0DT2H4M24S**
test_time_fall_type3_redost_type3_redodt: DIFF: 2010-11-07 01:12:33 EDT - 2010-11-07 01:14:44 EST = **P-0Y0M0DT1H2M11S**
test_time_fall_type3_redost_type3_redost: DIFF: 2010-11-07 01:16:54 EST - 2010-11-07 01:14:44 EST = **P+0Y0M0DT0H2M10S**
test_time_fall_type3_redost_type3_st: DIFF: 2010-11-07 03:16:55 EST - 2010-11-07 01:14:44 EST = **P+0Y0M0DT2H2M11S**
test_time_fall_type3_redost_type3_post: DIFF: 2010-11-08 19:59:59 EST - 2010-11-07 01:14:44 EST = **P+0Y0M1DT18H45M15S**
test_time_fall_type3_st_type3_prev: DIFF: 2010-11-06 18:38:28 EDT - 2010-11-07 03:16:55 EST = **P-0Y0M0DT9H38M27S**
test_time_fall_type3_st_type3_dt: DIFF: 2010-11-07 00:10:20 EDT - 2010-11-07 03:16:55 EST = **P-0Y0M0DT4H6M35S**
test_time_fall_type3_st_type3_redodt: DIFF: 2010-11-07 01:12:33 EDT - 2010-11-07 03:16:55 EST = **P-0Y0M0DT3H4M22S**
test_time_fall_type3_st_type3_redost: DIFF: 2010-11-07 01:14:44 EST - 2010-11-07 03:16:55 EST = **P-0Y0M0DT2H2M11S**
test_time_fall_type3_st_type3_st: DIFF: 2010-11-07 05:19:56 EST - 2010-11-07 03:16:55 EST = **P+0Y0M0DT2H3M1S**
test_time_fall_type3_st_type3_post: DIFF: 2010-11-08 19:59:59 EST - 2010-11-07 03:16:55 EST = **P+0Y0M1DT16H43M4S**
test_time_fall_type3_post_type3_prev: DIFF: 2010-11-06 18:38:28 EDT - 2010-11-08 19:59:59 EST = **P-0Y0M2DT1H21M31S**
test_time_fall_type3_post_type3_dt: DIFF: 2010-11-07 00:10:20 EDT - 2010-11-08 19:59:59 EST = **P-0Y0M1DT19H49M39S**
test_time_fall_type3_post_type3_redodt: DIFF: 2010-11-07 01:12:33 EDT - 2010-11-08 19:59:59 EST = **P-0Y0M1DT18H47M26S**
test_time_fall_type3_post_type3_redost: DIFF: 2010-11-07 01:14:44 EST - 2010-11-08 19:59:59 EST = **P-0Y0M1DT18H45M15S**
test_time_fall_type3_post_type3_st: DIFF: 2010-11-07 03:16:55 EST - 2010-11-08 19:59:59 EST = **P-0Y0M1DT16H43M4S**
test_time_fall_type3_post_type3_post: DIFF: 2010-11-08 19:59:59 EST - 2010-11-08 18:57:55 EST = **P+0Y0M0DT1H2M4S**
test_time_fall_type3_dtsec_type3_stsec: DIFF: 2010-11-07 01:00:00 EST - 2010-11-07 01:59:59 EDT = **P+0Y0M0DT0H0M1S**
test_time_fall_type3_stsec_type3_dtsec: DIFF: 2010-11-07 01:59:59 EDT - 2010-11-07 01:00:00 EST = **P-0Y0M0DT0H0M1S**
