--TEST--
strtotime() function - a test to show the difference in behaviour between 'first' and '1', "second" and "2"...
--SKIPIF--
<?php if (!function_exists('strtotime')) echo "SKIP"; ?>
--FILE--
<?php
date_default_timezone_set('UTC');
/*
 * The first of December 2008 is a Monday.
 * The term "Monday December 2008" will be parsed as the first Monday in December 2008.
 */

/*
 * This is parsed as the "first following Monday OR the current day if it is a Monday"
 */
var_dump(date('Y-m-d', strtotime('1 Monday December 2008')));
/*
 * This is parsed as the "second following Monday OR the first following
 * Monday if the current day is a Monday"
 */
var_dump(date('Y-m-d', strtotime('2 Monday December 2008')));
/*
 * This is parsed as the "third following Monday OR the second following
 * Monday if the current day is a Monday"
 */
var_dump(date('Y-m-d', strtotime('3 Monday December 2008')));
/*
 * This is parsed as the "first following Monday after the first Monday in December"
 */
var_dump(date('Y-m-d', strtotime('first Monday December 2008')));
/*
 * This is parsed as the "second following Monday after the first Monday in December"
 */
var_dump(date('Y-m-d', strtotime('second Monday December 2008')));
/*
 * This is parsed as the "third following Monday after the first Monday in December"
 */
var_dump(date('Y-m-d', strtotime('third Monday December 2008')));
?>
--EXPECTF--
%string|unicode%(10) "2008-12-01"
%string|unicode%(10) "2008-12-08"
%string|unicode%(10) "2008-12-15"
%string|unicode%(10) "2008-12-08"
%string|unicode%(10) "2008-12-15"
%string|unicode%(10) "2008-12-22"
