--TEST--
Test cal_to_jd() function : error conditions
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
var_dump(cal_to_jd(-1, 8, 26, 74));
?>
--EXPECTF--
Warning: cal_to_jd(): invalid calendar ID -1 in %s on line %d
bool(false)
