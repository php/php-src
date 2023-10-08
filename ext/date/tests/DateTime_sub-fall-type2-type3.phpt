--TEST--
DateTime::sub() -- fall type2 type3
--CREDITS--
Daniel Convissor <danielc@php.net>
--FILE--
<?php

require 'examine_diff.inc';
define('PHPT_DATETIME_SHOW', PHPT_DATETIME_SHOW_SUB);
require 'DateTime_data-fall-type2-type3.inc';

?>
--EXPECT--
test_time_fall_type2_prev_type3_prev: SUB: 2010-11-06 18:38:28 EDT - P+0Y1M2DT16H19M40S = **2010-10-04 02:18:48 EDT**
test_time_fall_type2_prev_type3_dt: SUB: 2010-11-07 00:10:20 EDT - P+0Y0M0DT5H31M52S = **2010-11-06 18:38:28 EDT**
test_time_fall_type2_prev_type3_redodt: SUB: 2010-11-07 01:12:33 EDT - P+0Y0M0DT6H34M5S = **2010-11-06 18:38:28 EDT**
test_time_fall_type2_prev_type3_redost: SUB: 2010-11-07 01:14:44 EST - P+0Y0M0DT7H36M16S = **2010-11-06 18:38:28 EDT**
test_time_fall_type2_prev_type3_st: SUB: 2010-11-07 03:16:55 EST - P+0Y0M0DT9H38M27S = **2010-11-06 18:38:28 EDT**
test_time_fall_type2_prev_type3_post: SUB: 2010-11-08 19:59:59 EST - P+0Y0M2DT1H21M31S = **2010-11-06 18:38:28 EDT**
test_time_fall_type2_dt_type3_prev: SUB: 2010-11-06 18:38:28 EDT - P-0Y0M0DT5H31M52S = **2010-11-07 00:10:20 EDT**
test_time_fall_type2_dt_type3_dt: SUB: 2010-11-07 00:15:35 EDT - P+0Y0M0DT0H5M15S = **2010-11-07 00:10:20 EDT**
test_time_fall_type2_dt_type3_redodt: SUB: 2010-11-07 01:12:33 EDT - P+0Y0M0DT1H2M13S = **2010-11-07 00:10:20 EDT**
test_time_fall_type2_dt_type3_redost: SUB: 2010-11-07 01:14:44 EST - P+0Y0M0DT2H4M24S = **2010-11-07 00:10:20 EDT**
test_time_fall_type2_dt_type3_st: SUB: 2010-11-07 03:16:55 EST - P+0Y0M0DT4H6M35S = **2010-11-07 00:10:20 EDT**
test_time_fall_type2_dt_type3_post: SUB: 2010-11-08 19:59:59 EST - P+0Y0M1DT20H49M39S = **2010-11-07 00:10:20 EDT**
test_time_fall_type2_redodt_type3_prev: SUB: 2010-11-06 18:38:28 EDT - P-0Y0M0DT6H34M5S = **2010-11-07 01:12:33 EDT**
test_time_fall_type2_redodt_type3_dt: SUB: 2010-11-07 00:10:20 EDT - P-0Y0M0DT1H2M13S = **2010-11-07 01:12:33 EDT**
test_time_fall_type2_redodt_type3_redodt: SUB: 2010-11-07 01:15:35 EDT - P+0Y0M0DT0H3M2S = **2010-11-07 01:12:33 EDT**
test_time_fall_type2_redodt_type3_redost: SUB: 2010-11-07 01:14:44 EST - P+0Y0M0DT1H2M11S = **2010-11-07 01:12:33 EDT**
test_time_fall_type2_redodt_type3_st: SUB: 2010-11-07 03:16:55 EST - P+0Y0M0DT3H4M22S = **2010-11-07 01:12:33 EDT**
test_time_fall_type2_redodt_type3_post: SUB: 2010-11-08 19:59:59 EST - P+0Y0M1DT19H47M26S = **2010-11-07 01:12:33 EDT**
test_time_fall_type2_redost_type3_prev: SUB: 2010-11-06 18:38:28 EDT - P-0Y0M0DT7H36M16S = **2010-11-07 01:14:44 EST**
test_time_fall_type2_redost_type3_dt: SUB: 2010-11-07 00:10:20 EDT - P-0Y0M0DT2H4M24S = **2010-11-07 01:14:44 EST**
test_time_fall_type2_redost_type3_redodt: SUB: 2010-11-07 01:12:33 EDT - P-0Y0M0DT1H2M11S = **2010-11-07 01:14:44 EST**
test_time_fall_type2_redost_type3_redost: SUB: 2010-11-07 01:16:54 EST - P+0Y0M0DT0H2M10S = **2010-11-07 01:14:44 EST**
test_time_fall_type2_redost_type3_st: SUB: 2010-11-07 03:16:55 EST - P+0Y0M0DT2H2M11S = **2010-11-07 01:14:44 EST**
test_time_fall_type2_redost_type3_post: SUB: 2010-11-08 19:59:59 EST - P+0Y0M1DT18H45M15S = **2010-11-07 01:14:44 EST**
test_time_fall_type2_st_type3_prev: SUB: 2010-11-06 18:38:28 EDT - P-0Y0M0DT9H38M27S = **2010-11-07 03:16:55 EST**
test_time_fall_type2_st_type3_dt: SUB: 2010-11-07 00:10:20 EDT - P-0Y0M0DT4H6M35S = **2010-11-07 03:16:55 EST**
test_time_fall_type2_st_type3_redodt: SUB: 2010-11-07 01:12:33 EDT - P-0Y0M0DT3H4M22S = **2010-11-07 03:16:55 EST**
test_time_fall_type2_st_type3_redost: SUB: 2010-11-07 01:14:44 EST - P-0Y0M0DT2H2M11S = **2010-11-07 03:16:55 EST**
test_time_fall_type2_st_type3_st: SUB: 2010-11-07 05:19:56 EST - P+0Y0M0DT2H3M1S = **2010-11-07 03:16:55 EST**
test_time_fall_type2_st_type3_post: SUB: 2010-11-08 19:59:59 EST - P+0Y0M1DT16H43M4S = **2010-11-07 03:16:55 EST**
test_time_fall_type2_post_type3_prev: SUB: 2010-11-06 18:38:28 EDT - P-0Y0M2DT1H21M31S = **2010-11-08 19:59:59 EST**
test_time_fall_type2_post_type3_dt: SUB: 2010-11-07 00:10:20 EDT - P-0Y0M1DT20H49M39S = **2010-11-08 20:59:59 EST**
test_time_fall_type2_post_type3_redodt: SUB: 2010-11-07 01:12:33 EDT - P-0Y0M1DT19H47M26S = **2010-11-08 20:59:59 EST**
test_time_fall_type2_post_type3_redost: SUB: 2010-11-07 01:14:44 EST - P-0Y0M1DT18H45M15S = **2010-11-08 19:59:59 EST**
test_time_fall_type2_post_type3_st: SUB: 2010-11-07 03:16:55 EST - P-0Y0M1DT16H43M4S = **2010-11-08 19:59:59 EST**
test_time_fall_type2_post_type3_post: SUB: 2010-11-08 19:59:59 EST - P+0Y0M0DT1H2M4S = **2010-11-08 18:57:55 EST**
test_time_fall_type2_dtsec_type3_stsec: SUB: 2010-11-07 01:00:00 EST - P+0Y0M0DT0H0M1S = **2010-11-07 01:59:59 EDT**
test_time_fall_type2_stsec_type3_dtsec: SUB: 2010-11-07 01:59:59 EDT - P-0Y0M0DT0H0M1S = **2010-11-07 01:00:00 EST**
