--TEST--
DateTime::add() -- spring type2 type3
--CREDITS--
Daniel Convissor <danielc@php.net>
--FILE--
<?php

require 'examine_diff.inc';
define('PHPT_DATETIME_SHOW', PHPT_DATETIME_SHOW_ADD);
require 'DateTime_data-spring-type2-type3.inc';

?>
--EXPECT--
test_time_spring_type2_prev_type3_prev: ADD: 2010-02-11 02:18:48 EST + P+0Y1M2DT16H19M40S = **2010-03-13 18:38:28 EST**
test_time_spring_type2_prev_type3_st: ADD: 2010-03-13 18:38:28 EST + P+0Y0M0DT5H31M52S = **2010-03-14 00:10:20 EST**
test_time_spring_type2_prev_type3_dt: ADD: 2010-03-13 18:38:28 EST + P+0Y0M0DT7H38M27S = **2010-03-14 02:16:55 EST**
test_time_spring_type2_prev_type3_post: ADD: 2010-03-13 18:38:28 EST + P+0Y0M2DT1H21M31S = **2010-03-15 19:59:59 EST**
test_time_spring_type2_st_type3_prev: ADD: 2010-03-14 00:10:20 EST + P-0Y0M0DT5H31M52S = **2010-03-13 18:38:28 EST**
test_time_spring_type2_st_type3_st: ADD: 2010-03-14 00:10:20 EST + P+0Y0M0DT0H5M15S = **2010-03-14 00:15:35 EST**
test_time_spring_type2_st_type3_dt: ADD: 2010-03-14 00:10:20 EST + P+0Y0M0DT2H6M35S = **2010-03-14 02:16:55 EST**
test_time_spring_type2_st_type3_post: ADD: 2010-03-14 00:10:20 EST + P+0Y0M1DT18H49M39S = **2010-03-15 18:59:59 EST**
test_time_spring_type2_dt_type3_prev: ADD: 2010-03-14 03:16:55 EDT + P-0Y0M0DT7H38M27S = **2010-03-13 19:38:28 EDT**
test_time_spring_type2_dt_type3_st: ADD: 2010-03-14 03:16:55 EDT + P-0Y0M0DT2H6M35S = **2010-03-14 01:10:20 EDT**
test_time_spring_type2_dt_type3_dt: ADD: 2010-03-14 03:16:55 EDT + P+0Y0M0DT2H3M1S = **2010-03-14 05:19:56 EDT**
test_time_spring_type2_dt_type3_post: ADD: 2010-03-14 03:16:55 EDT + P+0Y0M1DT16H43M4S = **2010-03-15 19:59:59 EDT**
test_time_spring_type2_post_type3_prev: ADD: 2010-03-15 19:59:59 EDT + P-0Y0M2DT1H21M31S = **2010-03-13 18:38:28 EDT**
test_time_spring_type2_post_type3_st: ADD: 2010-03-15 19:59:59 EDT + P-0Y0M1DT18H49M39S = **2010-03-14 01:10:20 EDT**
test_time_spring_type2_post_type3_dt: ADD: 2010-03-15 19:59:59 EDT + P-0Y0M1DT16H43M4S = **2010-03-14 03:16:55 EDT**
test_time_spring_type2_post_type3_post: ADD: 2010-03-15 18:57:55 EDT + P+0Y0M0DT1H2M4S = **2010-03-15 19:59:59 EDT**
test_time_spring_type2_stsec_type3_dtsec: ADD: 2010-03-14 01:59:59 EST + P+0Y0M0DT0H0M1S = **2010-03-14 02:00:00 EST**
test_time_spring_type2_dtsec_type3_stsec: ADD: 2010-03-14 03:00:00 EDT + P-0Y0M0DT0H0M1S = **2010-03-14 02:59:59 EDT**
