--TEST--
DateTime::diff() days -- fall type2 type3
--CREDITS--
Daniel Convissor <danielc@php.net>
--FILE--
<?php

require 'examine_diff.inc';
define('PHPT_DATETIME_SHOW', PHPT_DATETIME_SHOW_DAYS);
require 'DateTime_data-fall-type2-type3.inc';

?>
--EXPECT--
test_time_fall_type2_prev_type3_prev: DAYS: **33**
test_time_fall_type2_prev_type3_dt: DAYS: **0**
test_time_fall_type2_prev_type3_redodt: DAYS: **0**
test_time_fall_type2_prev_type3_redost: DAYS: **0**
test_time_fall_type2_prev_type3_st: DAYS: **0**
test_time_fall_type2_prev_type3_post: DAYS: **2**
test_time_fall_type2_dt_type3_prev: DAYS: **0**
test_time_fall_type2_dt_type3_dt: DAYS: **0**
test_time_fall_type2_dt_type3_redodt: DAYS: **0**
test_time_fall_type2_dt_type3_redost: DAYS: **0**
test_time_fall_type2_dt_type3_st: DAYS: **0**
test_time_fall_type2_dt_type3_post: DAYS: **1**
test_time_fall_type2_redodt_type3_prev: DAYS: **0**
test_time_fall_type2_redodt_type3_dt: DAYS: **0**
test_time_fall_type2_redodt_type3_redodt: DAYS: **0**
test_time_fall_type2_redodt_type3_redost: DAYS: **0**
test_time_fall_type2_redodt_type3_st: DAYS: **0**
test_time_fall_type2_redodt_type3_post: DAYS: **1**
test_time_fall_type2_redost_type3_prev: DAYS: **0**
test_time_fall_type2_redost_type3_dt: DAYS: **0**
test_time_fall_type2_redost_type3_redodt: DAYS: **0**
test_time_fall_type2_redost_type3_redost: DAYS: **0**
test_time_fall_type2_redost_type3_st: DAYS: **0**
test_time_fall_type2_redost_type3_post: DAYS: **1**
test_time_fall_type2_st_type3_prev: DAYS: **0**
test_time_fall_type2_st_type3_dt: DAYS: **0**
test_time_fall_type2_st_type3_redodt: DAYS: **0**
test_time_fall_type2_st_type3_redost: DAYS: **0**
test_time_fall_type2_st_type3_st: DAYS: **0**
test_time_fall_type2_st_type3_post: DAYS: **1**
test_time_fall_type2_post_type3_prev: DAYS: **2**
test_time_fall_type2_post_type3_dt: DAYS: **1**
test_time_fall_type2_post_type3_redodt: DAYS: **1**
test_time_fall_type2_post_type3_redost: DAYS: **1**
test_time_fall_type2_post_type3_st: DAYS: **1**
test_time_fall_type2_post_type3_post: DAYS: **0**
test_time_fall_type2_dtsec_type3_stsec: DAYS: **0**
test_time_fall_type2_stsec_type3_dtsec: DAYS: **0**
