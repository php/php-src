--TEST--
DateTime::diff() -- spring type2 type3
--CREDITS--
Daniel Convissor <danielc@php.net>
--FILE--
<?php

require 'examine_diff.inc';
define('PHPT_DATETIME_SHOW', PHPT_DATETIME_SHOW_DIFF);
require 'DateTime_data-spring-type2-type3.inc';

?>
--EXPECT--
test_time_spring_type2_prev_type3_prev: DIFF: 2010-03-13 18:38:28 EST - 2010-02-11 02:18:48 EST = **P+0Y1M2DT16H19M40S**
test_time_spring_type2_prev_type3_st: DIFF: 2010-03-14 00:10:20 EST - 2010-03-13 18:38:28 EST = **P+0Y0M0DT5H31M52S**
test_time_spring_type2_prev_type3_dt: DIFF: 2010-03-14 03:16:55 EDT - 2010-03-13 18:38:28 EST = **P+0Y0M0DT7H38M27S**
test_time_spring_type2_prev_type3_post: DIFF: 2010-03-15 19:59:59 EDT - 2010-03-13 18:38:28 EST = **P+0Y0M2DT0H21M31S**
test_time_spring_type2_st_type3_prev: DIFF: 2010-03-13 18:38:28 EST - 2010-03-14 00:10:20 EST = **P-0Y0M0DT5H31M52S**
test_time_spring_type2_st_type3_st: DIFF: 2010-03-14 00:15:35 EST - 2010-03-14 00:10:20 EST = **P+0Y0M0DT0H5M15S**
test_time_spring_type2_st_type3_dt: DIFF: 2010-03-14 03:16:55 EDT - 2010-03-14 00:10:20 EST = **P+0Y0M0DT2H6M35S**
test_time_spring_type2_st_type3_post: DIFF: 2010-03-15 19:59:59 EDT - 2010-03-14 00:10:20 EST = **P+0Y0M1DT18H49M39S**
test_time_spring_type2_dt_type3_prev: DIFF: 2010-03-13 18:38:28 EST - 2010-03-14 03:16:55 EDT = **P-0Y0M0DT7H38M27S**
test_time_spring_type2_dt_type3_st: DIFF: 2010-03-14 00:10:20 EST - 2010-03-14 03:16:55 EDT = **P-0Y0M0DT2H6M35S**
test_time_spring_type2_dt_type3_dt: DIFF: 2010-03-14 05:19:56 EDT - 2010-03-14 03:16:55 EDT = **P+0Y0M0DT2H3M1S**
test_time_spring_type2_dt_type3_post: DIFF: 2010-03-15 19:59:59 EDT - 2010-03-14 03:16:55 EDT = **P+0Y0M1DT16H43M4S**
test_time_spring_type2_post_type3_prev: DIFF: 2010-03-13 18:38:28 EST - 2010-03-15 19:59:59 EDT = **P-0Y0M2DT0H21M31S**
test_time_spring_type2_post_type3_st: DIFF: 2010-03-14 00:10:20 EST - 2010-03-15 19:59:59 EDT = **P-0Y0M1DT18H49M39S**
test_time_spring_type2_post_type3_dt: DIFF: 2010-03-14 03:16:55 EDT - 2010-03-15 19:59:59 EDT = **P-0Y0M1DT16H43M4S**
test_time_spring_type2_post_type3_post: DIFF: 2010-03-15 19:59:59 EDT - 2010-03-15 18:57:55 EDT = **P+0Y0M0DT1H2M4S**
test_time_spring_type2_stsec_type3_dtsec: DIFF: 2010-03-14 03:00:00 EDT - 2010-03-14 01:59:59 EST = **P+0Y0M0DT0H0M1S**
test_time_spring_type2_dtsec_type3_stsec: DIFF: 2010-03-14 01:59:59 EST - 2010-03-14 03:00:00 EDT = **P-0Y0M0DT0H0M1S**
